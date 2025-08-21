#include "box.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

Box::Box(float size, int level) : size(size), tessellation(level), modelMatrix(1.0f) {
    generateMesh();
}

void Box::generateMesh() {
    vertices.clear();
    indices.clear();

    int divisions = 1 << (tessellation - 1); // 1,2,4,8,16 depending on level
    float step = size / divisions;
    float half = size / 2.0f;

    // generate 6 faces
    // Each face lies on a constant coordinate plane
    auto addFace = [&](glm::vec3 origin, glm::vec3 u, glm::vec3 v, glm::vec3 normal) {
        int baseIndex = vertices.size();
        for (int i = 0; i < divisions; i++) {
            for (int j = 0; j < divisions; j++) {
                glm::vec3 v0 = origin + (i * step - half) * u + (j * step - half) * v;
                glm::vec3 v1 = origin + ((i + 1) * step - half) * u + (j * step - half) * v;
                glm::vec3 v2 = origin + ((i + 1) * step - half) * u + ((j + 1) * step - half) * v;
                glm::vec3 v3 = origin + (i * step - half) * u + ((j + 1) * step - half) * v;

                // push vertices with normal
                vertices.push_back({v0, normal});
                vertices.push_back({v1, normal});
                vertices.push_back({v2, normal});
                vertices.push_back({v3, normal});

                int idx = baseIndex + (i * divisions + j) * 4;
                indices.push_back(idx);
                indices.push_back(idx + 1);
                indices.push_back(idx + 2);
                indices.push_back(idx);
                indices.push_back(idx + 2);
                indices.push_back(idx + 3);
            }
        }
    };

    // +X, -X, +Y, -Y, +Z, -Z faces
    addFace(glm::vec3(half, 0, 0), glm::vec3(0, step, 0), glm::vec3(0, 0, step), glm::vec3(1, 0, 0));
    addFace(glm::vec3(-half, 0, 0), glm::vec3(0, step, 0), glm::vec3(0, 0, step), glm::vec3(-1, 0, 0));
    addFace(glm::vec3(0, half, 0), glm::vec3(step, 0, 0), glm::vec3(0, 0, step), glm::vec3(0, 1, 0));
    addFace(glm::vec3(0, -half, 0), glm::vec3(step, 0, 0), glm::vec3(0, 0, step), glm::vec3(0, -1, 0));
    addFace(glm::vec3(0, 0, half), glm::vec3(step, 0, 0), glm::vec3(0, step, 0), glm::vec3(0, 0, 1));
    addFace(glm::vec3(0, 0, -half), glm::vec3(step, 0, 0), glm::vec3(0, step, 0), glm::vec3(0, 0, -1));
}

// centroid helper
glm::vec3 Box::centroid() const {
    return glm::vec3(0.0f); // symmetric box centered at origin
}

// transformations about centroid
void Box::translate(const glm::vec3 &axis, float dist) {
    modelMatrix = glm::translate(modelMatrix, axis * dist);
}

void Box::scale(const glm::vec3 &axis, float factor) {
    glm::vec3 c = centroid();
    modelMatrix = glm::translate(modelMatrix, c);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f) + axis * (factor - 1.0f));
    modelMatrix = glm::translate(modelMatrix, -c);
}

void Box::rotate(const glm::vec3 &axis, float angleDeg) {
    glm::vec3 c = centroid();
    modelMatrix = glm::translate(modelMatrix, c);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angleDeg), axis);
    modelMatrix = glm::translate(modelMatrix, -c);
}

const glm::mat4 &Box::getModelMatrix() const {
    return modelMatrix;
}
