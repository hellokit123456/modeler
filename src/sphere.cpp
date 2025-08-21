#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

enum shape_type { SPHERE_SHAPE, CYLINDER_SHAPE, BOX_SHAPE, CONE_SHAPE };

class shape_t {
public:
    virtual void draw() = 0;
    virtual void scale(char axis, float factor) = 0;
    virtual void translate(char axis, float amount) = 0;
    virtual void rotate(char axis, float degrees) = 0;
    virtual void setColor(float r, float g, float b) = 0;

    std::vector<glm::vec4> vertices;
    std::vector<glm::vec4> colors;
    shape_type shapetype;
    unsigned int level;
    glm::vec3 scaleFactors = glm::vec3(1.0f,1.0f,1.0f);
    glm::vec3 translation = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // in degrees

    virtual ~shape_t(){}
};

class sphere_t : public shape_t {
private:
    std::vector<glm::vec4> baseVertices; // unit sphere vertices
    glm::vec4 color = glm::vec4(1.0f);   // default white

public:
    sphere_t(unsigned int lvl = 1) {
        if(lvl < 1) lvl = 1;
        if(lvl > 4) lvl = 4;
        level = lvl;
        shapetype = SPHERE_SHAPE;
        generateVertices();
        updateVertices();
    }

    void generateVertices() {
        baseVertices.clear();
        int latDiv, longDiv;
        switch(level) {
            case 1: latDiv=8;  longDiv=16; break;
            case 2: latDiv=16; longDiv=32; break;
            case 3: latDiv=32; longDiv=64; break;
            case 4: latDiv=64; longDiv=128; break;
        }

        for(int i=0;i<latDiv;i++){
            float theta1 = M_PI * float(i)/latDiv;
            float theta2 = M_PI * float(i+1)/latDiv;
            for(int j=0;j<longDiv;j++){
                float phi1 = 2*M_PI*float(j)/longDiv;
                float phi2 = 2*M_PI*float(j+1)/longDiv;

                glm::vec4 v1(std::sin(theta1)*std::cos(phi1), std::cos(theta1), std::sin(theta1)*std::sin(phi1),1.0f);
                glm::vec4 v2(std::sin(theta2)*std::cos(phi1), std::cos(theta2), std::sin(theta2)*std::sin(phi1),1.0f);
                glm::vec4 v3(std::sin(theta1)*std::cos(phi2), std::cos(theta1), std::sin(theta1)*std::sin(phi2),1.0f);
                glm::vec4 v4(std::sin(theta2)*std::cos(phi2), std::cos(theta2), std::sin(theta2)*std::sin(phi2),1.0f);

                baseVertices.push_back(v1); baseVertices.push_back(v2); baseVertices.push_back(v3);
                baseVertices.push_back(v3); baseVertices.push_back(v2); baseVertices.push_back(v4);
            }
        }
    }

    void updateVertices() {
        vertices.clear();
        colors.clear();
        for(auto &v : baseVertices){
            glm::vec4 scaled;
            scaled.x = v.x * scaleFactors.x + translation.x;
            scaled.y = v.y * scaleFactors.y + translation.y;
            scaled.z = v.z * scaleFactors.z + translation.z;
            scaled.w = 1.0f;
            vertices.push_back(scaled);
            colors.push_back(color);
        }
    }

    void scale(char axis, float factor) override {
        if(axis=='X') scaleFactors.x *= factor;
        else if(axis=='Y') scaleFactors.y *= factor;
        else if(axis=='Z') scaleFactors.z *= factor;
        updateVertices();
    }

    void translate(char axis, float amount) override {
        if(axis=='X') translation.x += amount;
        else if(axis=='Y') translation.y += amount;
        else if(axis=='Z') translation.z += amount;
        updateVertices();
    }

    void rotate(char axis, float degrees) override {
        if(axis=='X') rotation.x += degrees;
        else if(axis=='Y') rotation.y += degrees;
        else if(axis=='Z') rotation.z += degrees;
        // rotation implementation would involve updating model matrix in OpenGL rendering
        // for simplicity, vertices are kept in local coordinates
    }

    void setColor(float r, float g, float b) override {
        color = glm::vec4(r,g,b,1.0f);
        updateVertices();
    }

    void draw() override {
        // In actual OpenGL: send vertices/colors to buffer and draw triangles
        std::cout << "Drawing Sphere with " << vertices.size()/3 << " triangles.\n";
    }
};
