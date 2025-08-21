#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>
#include <stack>

#include "sphere.cpp"
#include "cylinder.cpp"
#include "box.cpp"
#include "cone.cpp"

class HNode {
public:
    std::shared_ptr<Shape> shape;
    std::vector<std::shared_ptr<HNode>> children;

    HNode(std::shared_ptr<Shape> s) : shape(s) {}
};

class Model {
public:
    std::shared_ptr<HNode> root = nullptr;

    void draw() { drawNode(root); }

    // --- Saving ---
    void save(const std::string &filename) {
        std::ofstream out(filename);
        if (!out) { std::cerr << "Cannot open file to write\n"; return; }
        out << "# MyModel Hierarchy v1\n";
        saveNode(out, root, 0);
        std::cout << "Model saved to " << filename << "\n";
    }

    // --- Loading ---
    void load(const std::string &filename) {
        std::ifstream in(filename);
        if (!in) { std::cerr << "Cannot open file to read\n"; return; }

        std::stack<std::shared_ptr<HNode>> nodeStack;
        std::shared_ptr<HNode> currentNode = nullptr;
        root = nullptr;

        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0]=='#') continue;
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token=="NODE") {
                std::string type; unsigned int level; 
                float tx,ty,tz,rx,ry,rz,sx,sy,sz,r,g,b;
                iss >> type >> level >> tx >> ty >> tz >> rx >> ry >> rz >> sx >> sy >> sz >> r >> g >> b;

                std::shared_ptr<Shape> s = nullptr;
                if(type=="SPHERE") s = std::make_shared<Sphere>(1.0f,level);
                else if(type=="BOX") s = std::make_shared<Box>(1.0f,level);
                else if(type=="CYLINDER") s = std::make_shared<Cylinder>(1.0f,1.0f,level);
                else if(type=="CONE") s = std::make_shared<Cone>(1.0f,1.0f,level);

                if(s) {
                    s->translate('X',tx); s->translate('Y',ty); s->translate('Z',tz);
                    s->rotate('X',rx); s->rotate('Y',ry); s->rotate('Z',rz);
                    s->scale('X',sx); s->scale('Y',sy); s->scale('Z',sz);
                    s->setColor(glm::vec3(r,g,b));
                }

                std::shared_ptr<HNode> node = std::make_shared<HNode>(s);
                if (!root) root = node;
                if(currentNode) currentNode->children.push_back(node);
                nodeStack.push(node);
                currentNode = node;
            } 
            else if(token=="CHILD") { /* just a marker */ }
            else if(token=="ENDCHILD") { if(!nodeStack.empty()) nodeStack.pop(); if(!nodeStack.empty()) currentNode=nodeStack.top(); else currentNode=nullptr; }
            else if(token=="ENDNODE") { if(!nodeStack.empty()) nodeStack.pop(); if(!nodeStack.empty()) currentNode=nodeStack.top(); else currentNode=nullptr; }
        }

        std::cout << "Model loaded from " << filename << "\n";
    }

private:
    void drawNode(std::shared_ptr<HNode> node) {
        if(!node) return;
        if(node->shape) node->shape->draw();
        for(auto &child : node->children) drawNode(child);
    }

    void saveNode(std::ofstream &out, std::shared_ptr<HNode> node, int indent) {
        if(!node) return;
        std::string ind(indent*2,' ');

        auto &s = node->shape;
        if(!s) return;

        glm::vec3 col = s->getColor();
        glm::vec3 scale = s->getScale();
        glm::vec3 trans = s->getTranslation();
        glm::vec3 rot = s->getRotation();

        out << ind << "NODE " << s->getTypeName() << " " << s->getLevel() << " "
            << trans.x << " " << trans.y << " " << trans.z << " "
            << rot.x << " " << rot.y << " " << rot.z << " "
            << scale.x << " " << scale.y << " " << scale.z << " "
            << col.r << " " << col.g << " " << col.b << "\n";

        if(!node->children.empty()) {
            out << ind << "CHILD\n";
            for(auto &child : node->children) saveNode(out, child, indent+1);
            out << ind << "ENDCHILD\n";
        }

        out << ind << "ENDNODE\n";
    }
};
