#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <iostream>

// Forward declare Shape if not included yet
class Shape;

class HNode {
public:
    HNode(std::shared_ptr<Shape> s = nullptr)
        : shape(s), translation(0.0f), rotation(0.0f), scale(1.0f)
    {}

    void addChild(std::shared_ptr<HNode> child) {
        children.push_back(child);
    }

    void setTranslation(const glm::vec3& t) { translation = t; updateModel(); }
    void setRotation(const glm::vec3& r) { rotation = r; updateModel(); }
    void setScale(const glm::vec3& s) { scale = s; updateModel(); }

    void draw(const glm::mat4& parentTransform = glm::mat4(1.0f)) {
        glm::mat4 globalTransform = parentTransform * model;
        if (shape) {
            shape->setModelMatrix(globalTransform); // shape must have a method to accept a model matrix
            shape->draw();
        }
        for (auto& c : children) {
            c->draw(globalTransform);
        }
    }

private:
    std::shared_ptr<Shape> shape;
    glm::vec3 translation;
    glm::vec3 rotation; // Euler angles in degrees
    glm::vec3 scale;
    glm::mat4 model = glm::mat4(1.0f);

    std::vector<std::shared_ptr<HNode>> children;

    void updateModel() {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, translation);
        m = glm::rotate(m, glm::radians(rotation.x), glm::vec3(1,0,0));
        m = glm::rotate(m, glm::radians(rotation.y), glm::vec3(0,1,0));
        m = glm::rotate(m, glm::radians(rotation.z), glm::vec3(0,0,1));
        m = glm::scale(m, scale);
        model = m;
    }
};
