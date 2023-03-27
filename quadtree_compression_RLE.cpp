#include <iostream>
#include <vector>

struct QuadtreeNode {
    float x;
    float y;
    float size;
    int value;
    bool isLeaf;
    QuadtreeNode* children[4];
};

void compressRLE(std::vector<int>& values) {
    std::vector<int> compressed;
    int count = 1;
    int currentValue = values[0];

    for (int i = 1; i < values.size(); i++) {
        if (values[i] == currentValue) {
            count++;
        } else {
            compressed.push_back(currentValue);
            compressed.push_back(count);
            count = 1;
            currentValue = values[i];
        }
    }

    compressed.push_back(currentValue);
    compressed.push_back(count);

    values = compressed;
}

int main() {
    QuadtreeNode* root = new QuadtreeNode();
    root->x = 0;
    root->y = 0;
    root->size = 10;
    root->isLeaf = false;

    QuadtreeNode* child1 = new QuadtreeNode();
    child1->x = -2.5;
    child1->y = -2.5;
    child1->size = 5;
    child1->value = 0;
    child1->isLeaf = true;

    QuadtreeNode* child2 = new QuadtreeNode();
    child2->x = 2.5;
    child2->y = -2.5;
    child2->size = 5;
    child2->value = 1;
    child2->isLeaf = true;

    QuadtreeNode* child3 = new QuadtreeNode();
    child3->x = -2.5;
    child3->y = 2.5;
    child3->size = 5;
    child3->value = 1;
    child3->isLeaf = true;

    QuadtreeNode* child4 = new QuadtreeNode();
    child4->x = 2.5;
    child4->y = 2.5;
    child4->size = 5;
    child4->value = 1;
    child4->isLeaf = true;

    root->children[0] = child1;
    root->children[1] = child2;
    root->children[2] = child3;
    root->children[3] = child4;

    std::vector<int> values;
    std::cout<<"Init";
    // Traverse the quadtree in depth-first order
    std::vector<QuadtreeNode*> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        QuadtreeNode* current = stack.back();
        stack.pop_back();

        if (current->isLeaf) {
            values.push_back(current->value);
        } else {
            for (int i = 3; i >= 0; i--) {
                stack.push_back(current->children[i]);
            }
        }
    }

    // Compress the values using run-length encoding
    compressRLE(values);

    // Output the compressed values
    std::cout << "Compressed values: ";
    for (int i = 0; i < values.size(); i++) {
        std::cout << values[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
