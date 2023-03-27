#include <iostream>
#include <vector>
using namespace std;
const int MAX_CAPACITY = 4;

class Point {
public:
    double x, y;
    double elevation;
    Point(double x_, double y_, double val) : x(x_), y(y_), elevation(val) {}
};

class Rectangle {
public:
    double x, y, width, height;
    Rectangle(double x_, double y_, double w_, double h_) : x(x_), y(y_), width(w_), height(h_) {}
    bool contains(Point p) {
        return (p.x >= x - width && p.x <= x + width && p.y >= y - height && p.y <= y + height);
    }
    bool intersects(const Rectangle& other) const {
        return (x + width >= other.x && x <= other.x + other.width && y + height >= other.y && y <= other.y + other.height);
    }
};

class Quadtree {
private:
    Rectangle boundary;
    std::vector<Point> points;
    Quadtree *northwest, *northeast, *southwest, *southeast;
    bool divided;
public:
    Quadtree(Rectangle boundary_) : boundary(boundary_), points(), divided(false), northwest(nullptr), northeast(nullptr), southwest(nullptr), southeast(nullptr) {}
    ~Quadtree() {
        delete northwest;
        delete northeast;
        delete southwest;
        delete southeast;
    }
    void insert(Point p) {
        if (!boundary.contains(p)) {
            return;
        }
        if (points.size() < MAX_CAPACITY) {
            points.push_back(p);
            return;
        }
        if (!divided) {
            subdivide();
        }
        northwest->insert(p);
        northeast->insert(p);
        southwest->insert(p);
        southeast->insert(p);
    }
    void subdivide() {
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
    }
    vector<Point> intersect(Rectangle rect) {
    vector<Point> result;
    if (!boundary.intersects(rect)) {
        return result;
    }
    for (auto& point : points) {
        if (rect.contains(point)) {
            result.push_back(point);
        }
    }
    if (northwest != nullptr) {
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
    void query(Rectangle range, std::vector<Point>& found) {
        if (!boundary.intersects(range)) {
            return;
        }
        for (auto p : points) {
            if (range.contains(p)) {
                found.push_back(p);
            }
        }
        if (divided) {
            northwest->query(range, found);
            northeast->query(range, found);
            southwest->query(range, found);
            southeast->query(range, found);
        }
    }
};
int main() {
    Rectangle boundary(-100, -100, 200, 200);
    Quadtree qt(boundary);
    qt.insert(Point(1, 2,0.0));
    qt.insert(Point(-3, 4,10.0));
    qt.insert(Point(10, 20,2.0));
    qt.insert(Point(-30, -40,7.0));
    std::vector<Point> found;
    Rectangle range(-5, -5, 10, 10);
    qt.query(range, found);
    //cout<<"Just before loop";
    for (auto p : found) {
        std::cout << "(" << p.x << ", " << p.y << ")" << p.elevation << std::endl;
    }
    return 0;
}
//Normal Quadtree
