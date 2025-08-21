#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <sstream>

class Cylinder : public Shape {
public:
    Cylinder(float radius, float height, unsigned int level) {
        this->shapetype = ShapeType::CYLINDER_SHAPE;
        this->level = std::min(std::max(level, 1u), 4u);
        this->radius = radius;
        this->height = height;
        centroid = glm::vec3(0.0f, 0.0f, 0.0f);
        generateMesh();
        model = glm::mat4(1.0f);
        color = glm::vec3(1.0f,1.0f,1.0f);
    }

    void draw() override {
        // Use your OpenGL code to draw using vertices & colors
        // Apply model matrix for transform
    }

    void translate(char axis, float val) override {
        if(axis=='X') centroid.x += val;
        else if(axis=='Y') centroid.y += val;
        else if(axis=='Z') centroid.z += val;
        model = glm::translate(model, (axis=='X'?glm::vec3(val,0,0):(axis=='Y'?glm::vec3(0,val,0):glm::vec3(0,0,val))));
    }

    void rotate(char axis, float angleDeg) override {
        if(axis=='X') model = glm::translate(model, centroid) * glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(1,0,0)) * glm::translate(glm::mat4(1.0f), -centroid) * model;
        if(axis=='Y') model = glm::translate(model, centroid) * glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(0,1,0)) * glm::translate(glm::mat4(1.0f), -centroid) * model;
        if(axis=='Z') model = glm::translate(model, centroid) * glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(0,0,1)) * glm::translate(glm::mat4(1.0f), -centroid) * model;
    }

    void scale(char axis, float factor) override {
        if(axis=='X') radius *= factor;
        else if(axis=='Y') height *= factor;
        else if(axis=='Z') radius *= factor;
        generateMesh(); // regenerate vertices after scaling
    }

    void setColor(glm::vec3 col) override {
        color = col;
        for(auto& c : colors) c = glm::vec4(color,1.0f);
    }

    std::string serialize() override {
        std::ostringstream oss;
        oss << "CYLINDER " << radius << " " << height << " " << level << " " << color.r << " " << color.g << " " << color.b;
        return oss.str();
    }

private:
    float radius;
    float height;
    glm::vec3 centroid;
    glm::vec3 color;
    glm::mat4 model;

    void generateMesh() {
        vertices.clear();
        colors.clear();

        unsigned int baseDiv = 16 * (1<<(level-1)); // 16,32,64,128 triangles
        float halfH = height/2.0f;

        // Base cap
        for(unsigned int i=0;i<baseDiv;i++){
            float theta1 = 2.0f * M_PI * i / baseDiv;
            float theta2 = 2.0f * M_PI * (i+1) / baseDiv;

            glm::vec4 v0(0.0f,-halfH,0.0f,1.0f);
            glm::vec4 v1(radius*cos(theta1),-halfH,radius*sin(theta1),1.0f);
            glm::vec4 v2(radius*cos(theta2),-halfH,radius*sin(theta2),1.0f);

            vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
            colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f));
        }

        // Top cap
        for(unsigned int i=0;i<baseDiv;i++){
            float theta1 = 2.0f * M_PI * i / baseDiv;
            float theta2 = 2.0f * M_PI * (i+1) / baseDiv;

            glm::vec4 v0(0.0f,halfH,0.0f,1.0f);
            glm::vec4 v1(radius*cos(theta2),halfH,radius*sin(theta2),1.0f);
            glm::vec4 v2(radius*cos(theta1),halfH,radius*sin(theta1),1.0f);

            vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
            colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f));
        }

        // Curved surface
        for(unsigned int i=0;i<baseDiv;i++){
            float theta1 = 2.0f * M_PI * i / baseDiv;
            float theta2 = 2.0f * M_PI * (i+1) / baseDiv;

            glm::vec4 v1(radius*cos(theta1),-halfH,radius*sin(theta1),1.0f);
            glm::vec4 v2(radius*cos(theta2),-halfH,radius*sin(theta2),1.0f);
            glm::vec4 v3(radius*cos(theta1),halfH,radius*sin(theta1),1.0f);
            glm::vec4 v4(radius*cos(theta2),halfH,radius*sin(theta2),1.0f);

            // first triangle
            vertices.push_back(v1); vertices.push_back(v3); vertices.push_back(v2);
            colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f));
            // second triangle
            vertices.push_back(v2); vertices.push_back(v3); vertices.push_back(v4);
            colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f)); colors.push_back(glm::vec4(color,1.0f));
        }
    }
};
