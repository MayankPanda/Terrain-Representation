#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
const int MAX_CAPACITY = 4;

class Point
{
public:
    double x, y;
    double elevation;
    Point(double x_, double y_, double val) : x(x_), y(y_), elevation(val) {}
};

class Rectangle
{
public:
    double x, y, width, height;
    Rectangle(double x_, double y_, double w_, double h_) : x(x_), y(y_), width(w_), height(h_) {}
    bool contains(Point p)
    {
        return (p.x >= x - width && p.x <= x + width && p.y >= y - height && p.y <= y + height);
    }
    bool intersects(const Rectangle &other) const
    {
        return (x + width >= other.x && x <= other.x + other.width && y + height >= other.y && y <= other.y + other.height);
    }
};
class Quadtree
{
private:
    Rectangle boundary;
    std::vector<Point> points;
    Quadtree *northwest, *northeast, *southwest, *southeast;
    bool divided;
    bool compressed;

public:
    Quadtree(Rectangle boundary_) : boundary(boundary_), points(), divided(false), compressed(false), northwest(nullptr), northeast(nullptr), southwest(nullptr), southeast(nullptr) {}
    ~Quadtree()
    {
        delete northwest;
        delete northeast;
        delete southwest;
        delete southeast;
    }
    void insert(Point p)
    {
        if (!boundary.contains(p))
        {
            return;
        }
        if (compressed)
        {
            uncompress();
        }
        if (points.size() < MAX_CAPACITY)
        {
            points.push_back(p);
            return;
        }
        if (!divided)
        {
            subdivide();
        }
        northwest->insert(p);
        northeast->insert(p);
        southwest->insert(p);
        southeast->insert(p);
    }
    void subdivide()
    {
        double x = boundary.x;
        double y = boundary.y;
        double w = boundary.width / 2;
        double h = boundary.height / 2;
        Rectangle nw(x - w, y - h, w, h);
        northwest = new Quadtree(nw);
        Rectangle ne(x + w, y - h, w, h);
        northeast = new Quadtree(ne);
        Rectangle sw(x - w, y + h, w, h);
        southwest = new Quadtree(sw);
        Rectangle se(x + w, y + h, w, h);
        southeast = new Quadtree(se);
        divided = true;

        if (northwest != nullptr && northwest->compressed)
        {
            northwest->uncompress();
            for (auto &point : northwest->points)
            {
                insert(point);
            }
            delete northwest;
            northwest = nullptr;
        }
        if (northeast != nullptr && northeast->compressed)
        {
            northeast->uncompress();
            for (auto &point : northeast->points)
            {
                insert(point);
            }
            delete northeast;
            northeast = nullptr;
        }
        if (southwest != nullptr && southwest->compressed)
        {
            southwest->uncompress();
            for (auto &point : southwest->points)
            {
                insert(point);
            }
            delete southwest;
            southwest = nullptr;
        }
        if (southeast != nullptr && southeast->compressed)
        {
            southeast->uncompress();
            for (auto &point : southeast->points)
            {
                insert(point);
            }
            delete southeast;
            southeast = nullptr;
        }
    }
    void compress()
    {
        if (divided)
        {
            for (auto &child : {northwest, northeast, southwest, southeast})
            {
                child->compress();
            }
            if (northwest->compressed && northeast->compressed && southwest->compressed && southeast->compressed &&
                northwest->points.size() + northeast->points.size() + southwest->points.size() + southeast->points.size() < MAX_CAPACITY)
            {
                points.clear();
                for (auto &child : {northwest, northeast, southwest, southeast})
                {
                    points.insert(points.end(), child->points.begin(), child->points.end());
                    delete child;
                }
                divided = false;
                compressed = true;
            }
        }
    }
    void query(Rectangle range, std::vector<Point> &found)
    {
        if (!boundary.intersects(range))
        {
            return;
        }
        for (auto p : points)
        {
            if (range.contains(p))
            {
                found.push_back(p);
            }
        }
        if (divided)
        {
            northwest->query(range, found);
            northeast->query(range, found);
            southwest->query(range, found);
            southeast->query(range, found);
        }
    }
    vector<Point> intersect(Rectangle rect)
    {
        vector<Point> result;
        if (!boundary.intersects(rect))
        {
            return result;
        }
        for (auto &point : points)
        {
            if (rect.contains(point))
            {
                result.push_back(point);
            }
        }
        if (northwest != nullptr)
        {
            auto res_nw = northwest->intersect(rect);
            result.insert(result.end(), res_nw.begin(), res_nw.end());
            auto res_ne = northeast->intersect(rect);
            result.insert(result.end(), res_ne.begin(), res_ne.end());
            auto res_sw = southwest->intersect(rect);
            result.insert(result.end(), res_sw.begin(), res_sw.end());
            auto res_se = southeast->intersect(rect);
            result.insert(result.end(), res_se.begin(), res_se.end());
        }
        return result;
    }
    void uncompress()
    {
        if (compressed)
        {
            for (auto &point : points)
            {
                insert(point);
            }
            points.clear();
            divided = true;
            compressed = false;
            northwest = new Quadtree(Rectangle(boundary.x - boundary.width / 4, boundary.y - boundary.height / 4, boundary.width / 2, boundary.height / 2));
            northeast = new Quadtree(Rectangle(boundary.x + boundary.width / 4, boundary.y - boundary.height / 4, boundary.width / 2, boundary.height / 2));
            southwest = new Quadtree(Rectangle(boundary.x - boundary.width / 4, boundary.y + boundary.height / 4, boundary.width / 2, boundary.height / 2));
            southeast = new Quadtree(Rectangle(boundary.x + boundary.width / 4, boundary.y + boundary.height / 4, boundary.width / 2, boundary.height / 2));
            for (auto &child : {northwest, northeast, southwest, southeast})
            {
                for (auto &point : points)
                {
                    child->insert(point);
                }
            }
        }
    }
    bool is_smooth(Rectangle rect, int j)
    {
        double w = rect.width / pow(2, j);
        double h = rect.height / pow(2, j);
        if (divided)
        {
            auto nw_smooth = northwest->is_smooth(rect, j);
            auto ne_smooth = northeast->is_smooth(rect, j);
            auto sw_smooth = southwest->is_smooth(rect, j);
            auto se_smooth = southeast->is_smooth(rect, j);
            return (nw_smooth && ne_smooth && sw_smooth && se_smooth);
        }
        else
        {
            // The node is a leaf node
            bool smooth = true;
            for (auto p : points)
            {
                Rectangle p_rect(p.x, p.y, w, h);
                if (!p_rect.intersects(rect))
                {
                    smooth = false;
                    break;
                }
            }
            return smooth;
        }
    }
};
int main()
{
    Rectangle boundary(-100, -100, 200, 200);
    Quadtree qt(boundary);
    std::string line;
    std::ifstream file("points.txt");
    if (file.is_open())
    {
        while (getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> words;
            std::string word;
            while (iss >> word)
            {
                words.push_back(word);
            }
            qt.insert(Point(stoi(words[0]), stoi(words[1]), stod(words[2])));
        }
        file.close();
    }
    // qt.insert(Point(1, 2,0.0));
    // qt.insert(Point(-3, 4,10.0));
    // qt.insert(Point(10, 20,2.0));
    // qt.insert(Point(-30, -40,7.0));
    std::vector<Point> found;
    Rectangle range(-5, -5, 10, 10);
    qt.query(range, found);
    for (auto p : found)
    {
        std::cout << "(" << p.x << ", " << p.y << ")" << p.elevation << std::endl;
    }

    return 0;
}
