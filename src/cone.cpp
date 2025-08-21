#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
class Cone {
public:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    glm::mat4 modelMatrix;

    Cone(int level = 1) {
        modelMatrix = glm::mat4(1.0f);
        generate(level);
    }

    void generate(int level) {
        vertices.clear();
        indices.clear();

        int slices = 16 * (1 << (level-1)); // 16,32,64,128
        float radius = 0.5f; // unit-ish
        float height = 1.0f;

        glm::vec3 apex(0.0f, height, 0.0f);
        glm::vec3 center(0.0f, 0.0f, 0.0f);

        // base circle vertices
        std::vector<glm::vec3> base;
        for (int i=0; i<slices; i++) {
            float theta = 2.0f * M_PI * i / slices;
            base.push_back(glm::vec3(radius*cos(theta), 0.0f, radius*sin(theta)));
        }

        // build side triangles
        for (int i=0; i<slices; i++) {
            glm::vec3 v0 = base[i];
            glm::vec3 v1 = base[(i+1)%slices];
            unsigned int start = vertices.size();
            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(apex);
            indices.push_back(start);
            indices.push_back(start+1);
            indices.push_back(start+2);
        }

        // build base (triangle fan)
        for (int i=0; i<slices; i++) {
            glm::vec3 v0 = base[i];
            glm::vec3 v1 = base[(i+1)%slices];
            unsigned int start = vertices.size();
            vertices.push_back(center);
            vertices.push_back(v0);
            vertices.push_back(v1);
            indices.push_back(start);
            indices.push_back(start+1);
            indices.push_back(start+2);
        }
    }

    void translate(const glm::vec3 &delta) {
        modelMatrix = glm::translate(modelMatrix, delta);
    }

    void rotate(const glm::vec3 &axis, float angleDegrees) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(angleDegrees), axis);
    }

    void scale(const glm::vec3 &factors) {
        modelMatrix = glm::scale(modelMatrix, factors);
    }
};

Cone::Cone(float radius, float height, int level)
    : radius(radius), height(height), level(level) {
    modelMatrix = glm::mat4(1.0f);
    generate(level);
}

void Cone::generate(int level) {
    vertices.clear();
    indices.clear();

    int baseDiv = 16 * (1 << (level - 1)); // 16, 32, 64, 128
    float angleStep = 2.0f * M_PI / baseDiv;

    // Apex of the cone
    glm::vec3 apex(0.0f, height, 0.0f);

    // Base center
    glm::vec3 baseCenter(0.0f, 0.0f, 0.0f);
    vertices.push_back(baseCenter);

    // Base vertices
    for (int i = 0; i < baseDiv; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.push_back(glm::vec3(x, 0.0f, z));
    }

    // Side faces
    for (int i = 1; i <= baseDiv; i++) {
        int next = (i % baseDiv) + 1;
        indices.push_back(apex);              // apex
        indices.push_back(vertices[i]);       // current base vertex
        indices.push_back(vertices[next]);    // next base vertex
    }

    // Base faces (triangle fan)
    for (int i = 1; i <= baseDiv; i++) {
        int next = (i % baseDiv) + 1;
        indices.push_back(baseCenter);
        indices.push_back(vertices[next]);
        indices.push_back(vertices[i]);
    }
}

// Apply transforms around base centroid
glm::vec3 Cone::getBaseCentroid() {
    return glm::vec3(0.0f, 0.0f, 0.0f); // base center is origin
}

void Cone::translate(char axis, float dist) {
    if (axis == 'x') modelMatrix = glm::translate(modelMatrix, glm::vec3(dist, 0, 0));
    if (axis == 'y') modelMatrix = glm::translate(modelMatrix, glm::vec3(0, dist, 0));
    if (axis == 'z') modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, dist));
}

void Cone::rotate(char axis, float angleDeg) {
    glm::vec3 pivot = getBaseCentroid();
    modelMatrix = glm::translate(modelMatrix, pivot);
    float angle = glm::radians(angleDeg);
    if (axis == 'x') modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(1,0,0));
    if (axis == 'y') modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0,1,0));
    if (axis == 'z') modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0,0,1));
    modelMatrix = glm::translate(modelMatrix, -pivot);
}

void Cone::scale(char axis, float factor) {
    glm::vec3 pivot = getBaseCentroid();
    modelMatrix = glm::translate(modelMatrix, pivot);
    if (axis == 'x') modelMatrix = glm::scale(modelMatrix, glm::vec3(factor,1,1));
    if (axis == 'y') modelMatrix = glm::scale(modelMatrix, glm::vec3(1,factor,1));
    if (axis == 'z') modelMatrix = glm::scale(modelMatrix, glm::vec3(1,1,factor));
    modelMatrix = glm::translate(modelMatrix, -pivot);
}

void Cone::draw(GLuint shaderProgram) {
    // Upload modelMatrix as uniform before drawing
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    // TODO: Bind VAO/VBO & issue draw calls with indices
}
