#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <sstream>

// Cylinder class inheriting from Shape (assume Shape has draw(), translate(), rotate(), scale(), setColor(), serialize() pure virtual)
class Cylinder : public Shape {
public:
    Cylinder(float radius, float height, unsigned int level) {
        this->shapetype = ShapeType::CYLINDER_SHAPE;
        this->level = std::min(std::max(level, 1u), 4u);

        baseRadius = radius;
        baseHeight = height;
        scaleFactors = glm::vec3(1.0f, 1.0f, 1.0f);
        centroid = glm::vec3(0.0f, 0.0f, 0.0f);
        color = glm::vec3(1.0f, 1.0f, 1.0f);

        generateBaseMesh(); // generate unscaled vertices
        updateVertices();   // apply initial scale
    }

    void draw() override {
        // Apply model matrix if needed and draw using vertices & colors
        // OpenGL drawing code here
    }

    void translate(char axis, float val) override {
        if(axis=='X') centroid.x += val;
        else if(axis=='Y') centroid.y += val;
        else if(axis=='Z') centroid.z += val;
        model = glm::translate(model, (axis=='X'?glm::vec3(val,0,0):(axis=='Y'?glm::vec3(0,val,0):glm::vec3(0,0,val))));
    }

    void rotate(char axis, float angleDeg) override {
        glm::mat4 rotMat = glm::mat4(1.0f);
        if(axis=='X') rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(1,0,0));
        else if(axis=='Y') rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(0,1,0));
        else if(axis=='Z') rotMat = glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(0,0,1));

        // rotate around centroid
        model = glm::translate(glm::mat4(1.0f), centroid) * rotMat * glm::translate(glm::mat4(1.0f), -centroid) * model;
    }

    void scale(char axis, float factor) override {
        if(axis=='X') scaleFactors.x *= factor;
        else if(axis=='Y') scaleFactors.y *= factor;
        else if(axis=='Z') scaleFactors.z *= factor;
        updateVertices();
    }

    void setColor(glm::vec3 col) override {
        color = col;
        for(auto& c : colors) c = glm::vec4(color, 1.0f);
    }

    std::string serialize() override {
        std::ostringstream oss;
        oss << "CYLINDER " << baseRadius << " " << baseHeight << " "
            << level << " "
            << scaleFactors.x << " " << scaleFactors.y << " " << scaleFactors.z << " "
            << color.r << " " << color.g << " " << color.b;
        return oss.str();
    }

private:
    float baseRadius;
    float baseHeight;
    glm::vec3 scaleFactors;
    glm::vec3 centroid;
    glm::vec3 color;
    glm::mat4 model;

    std::vector<glm::vec4> baseVertices; // unit cylinder mesh
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> colors;

    void generateBaseMesh() {
        baseVertices.clear();

        unsigned int baseDiv = 16 * (1<<(level-1)); // 16,32,64,128 triangles
        float halfH = baseHeight/2.0f;

        // Base cap
        for(unsigned int i=0;i<baseDiv;i++){
            float theta1 = 2.0f * M_PI * i / baseDiv;
            float theta2 = 2.0f * M_PI * (i+1) / baseDiv;

            baseVertices.push_back(glm::vec4(0.0f,-halfH,0.0f,1.0f));
            baseVertices.push_back(glm::vec4(baseRadius*cos(theta1),-halfH,baseRadius*sin(theta1),1.0f));
            baseVertices.push_back(glm::vec4(baseRadius*cos(theta2),-halfH,baseRadius*sin(theta2),1.0f));
        }

        // Top cap
        for(unsigned int i=0;i<baseDiv;i++){
            float theta1 = 2.0f * M_PI * i / baseDiv;
            float theta2 = 2.0f * M_PI * (i+1) / baseDiv;

            baseVertices.push_back(glm::vec4(0.0f,halfH,0.0f,1.0f));
            baseVertices.push_back(glm::vec4(baseRadius*cos(theta2),halfH,baseRadius*sin(theta2),1.0f));
            baseVertices.push_back(glm::vec4(baseRadius*cos(theta1),halfH,baseRadius*sin(theta1),1.0f));
        }

        // Curved surface
        for(unsigned int i=0;i<baseDiv;i++){
            float theta1 = 2.0f * M_PI * i / baseDiv;
            float theta2 = 2.0f * M_PI * (i+1) / baseDiv;

            glm::vec4 v1(baseRadius*cos(theta1),-halfH,baseRadius*sin(theta1),1.0f);
            glm::vec4 v2(baseRadius*cos(theta2),-halfH,baseRadius*sin(theta2),1.0f);
            glm::vec4 v3(baseRadius*cos(theta1),halfH,baseRadius*sin(theta1),1.0f);
            glm::vec4 v4(baseRadius*cos(theta2),halfH,baseRadius*sin(theta2),1.0f);

            // first triangle
            baseVertices.push_back(v1); baseVertices.push_back(v3); baseVertices.push_back(v2);
            // second triangle
            baseVertices.push_back(v2); baseVertices.push_back(v3); baseVertices.push_back(v4);
        }
    }

    void updateVertices() {
        vertices.clear();
        colors.clear();
        for(auto &v : baseVertices){
            glm::vec4 scaledV(v.x*scaleFactors.x, v.y*scaleFactors.y, v.z*scaleFactors.z, 1.0f);
            vertices.push_back(scaledV);
            colors.push_back(glm::vec4(color,1.0f));
        }
    }
};
