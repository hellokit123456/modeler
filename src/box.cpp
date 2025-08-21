#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Box {
public:
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    glm::mat4 modelMatrix;

    Box(int level = 1) {
        modelMatrix = glm::mat4(1.0f);
        generate(level);
    }

    void generate(int level) {
        vertices.clear();
        indices.clear();

        float half = 0.5f; // unit cube [-0.5,0.5]

        // base cube corners
        glm::vec3 corners[8] = {
            {-half,-half,-half}, { half,-half,-half},
            { half, half,-half}, {-half, half,-half},
            {-half,-half, half}, { half,-half, half},
            { half, half, half}, {-half, half, half}
        };

        int face[6][4] = {
            {0,1,2,3}, {4,5,6,7},
            {0,1,5,4}, {2,3,7,6},
            {1,2,6,5}, {0,3,7,4}
        };

        int divisions = 1 << (level-1); // 1,2,4,8
        float step = 1.0f / divisions;

        for (int f=0; f<6; f++) {
            glm::vec3 v0 = corners[face[f][0]];
            glm::vec3 v1 = corners[face[f][1]];
            glm::vec3 v2 = corners[face[f][2]];
            glm::vec3 v3 = corners[face[f][3]];

            for (int i=0; i<divisions; i++) {
                for (int j=0; j<divisions; j++) {
                    float u0 = i*step, u1=(i+1)*step;
                    float v0t= j*step, v1=(j+1)*step;

                    glm::vec3 p0 = (1-u0)*(1-v0t)*v0 + u0*(1-v0t)*v1 + u0*v0t*v2 + (1-u0)*v0t*v3;
                    glm::vec3 p1 = (1-u1)*(1-v0t)*v0 + u1*(1-v0t)*v1 + u1*v0t*v2 + (1-u1)*v0t*v3;
                    glm::vec3 p2 = (1-u1)*(1-v1)*v0 + u1*(1-v1)*v1 + u1*v1*v2 + (1-u1)*v1*v3;
                    glm::vec3 p3 = (1-u0)*(1-v1)*v0 + u0*(1-v1)*v1 + u0*v1*v2 + (1-u0)*v1*v3;

                    unsigned int start = vertices.size();
                    vertices.push_back(p0);
                    vertices.push_back(p1);
                    vertices.push_back(p2);
                    vertices.push_back(p3);

                    indices.push_back(start);
                    indices.push_back(start+1);
                    indices.push_back(start+2);

                    indices.push_back(start);
                    indices.push_back(start+2);
                    indices.push_back(start+3);
                }
            }
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

