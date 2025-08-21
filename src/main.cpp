#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

// Include all shapes
#include "sphere.cpp"
#include "cylinder.cpp"
#include "box.cpp"
#include "cone.cpp"

enum Mode { MODE_MODELLING, MODE_INSPECTION };
enum TransformMode { NONE, ROTATE, TRANSLATE, SCALE };

Mode currentMode = MODE_MODELLING;
TransformMode activeTransform = NONE;
char activeAxis = 'X';

std::vector<std::shared_ptr<Shape>> shapes;
std::shared_ptr<Shape> currentShape = nullptr;
int currentShapeIndex = -1;

// Camera globals
glm::vec3 camPos(0.0f, 0.0f, 5.0f);
glm::vec3 camFront(0.0f, 0.0f, -1.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = 0.0f;
float cameraSpeed = 0.1f;
float sensitivity = 2.5f;

glm::mat4 view;
glm::mat4 projection;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Camera movement
void moveCamera(GLFWwindow* window) {
    glm::vec3 right = glm::normalize(glm::cross(camFront, camUp));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos += cameraSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos -= cameraSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos -= cameraSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos += cameraSpeed * right;

    // Arrow keys for yaw/pitch
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) pitch += sensitivity * 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) pitch -= sensitivity * 0.1f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) yaw -= sensitivity * 0.5f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw += sensitivity * 0.5f;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(front);

    view = glm::lookAt(camPos, camPos + camFront, camUp);
}

// Save model
void saveModel(const std::string& filename) {
    std::ofstream out(filename);
    if (!out) { std::cerr << "Cannot open file to write.\n"; return; }
    for (auto& s : shapes) out << s->serialize() << "\n";
    std::cout << "Model saved to " << filename << "\n";
}

// Load model
void loadModel(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) { std::cerr << "Cannot open file to read.\n"; return; }

    shapes.clear();
    currentShape = nullptr;
    currentShapeIndex = -1;

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "SPHERE") shapes.push_back(std::make_shared<Sphere>(1.0f, 1));
        else if (type == "CYLINDER") shapes.push_back(std::make_shared<Cylinder>(1.0f, 1.0f, 1));
        else if (type == "BOX") shapes.push_back(std::make_shared<Box>(1.0f, 1));
        else if (type == "CONE") shapes.push_back(std::make_shared<Cone>(1.0f, 1.0f, 1));
        // TODO: Deserialize color + transforms
    }

    if (!shapes.empty()) { currentShapeIndex = 0; currentShape = shapes[0]; }
    std::cout << "Model loaded from " << filename << "\n";
}

// Switch active shape
void switchShape() {
    if (shapes.empty()) return;
    currentShapeIndex = (currentShapeIndex + 1) % shapes.size();
    currentShape = shapes[currentShapeIndex];
    std::cout << "Switched to shape " << currentShapeIndex + 1 << "\n";
}

// Key callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_M) { currentMode = MODE_MODELLING; std::cout << "MODELLING mode\n"; return; }
    if (key == GLFW_KEY_I) { currentMode = MODE_INSPECTION; std::cout << "INSPECTION mode\n"; return; }

    if (currentMode == MODE_MODELLING) {
        if (key == GLFW_KEY_1) { currentShape = std::make_shared<Sphere>(1.0f,1); shapes.push_back(currentShape); currentShapeIndex = shapes.size()-1; std::cout << "Added Sphere\n"; }
        if (key == GLFW_KEY_2) { currentShape = std::make_shared<Cylinder>(1.0f,1.0f,1); shapes.push_back(currentShape); currentShapeIndex = shapes.size()-1; std::cout << "Added Cylinder\n"; }
        if (key == GLFW_KEY_3) { currentShape = std::make_shared<Box>(1.0f,1); shapes.push_back(currentShape); currentShapeIndex = shapes.size()-1; std::cout << "Added Box\n"; }
        if (key == GLFW_KEY_4) { currentShape = std::make_shared<Cone>(1.0f,1.0f,1); shapes.push_back(currentShape); currentShapeIndex = shapes.size()-1; std::cout << "Added Cone\n"; }
        if (key == GLFW_KEY_5 && !shapes.empty()) { shapes.erase(shapes.begin() + currentShapeIndex); if(shapes.empty()) currentShape=nullptr; else currentShapeIndex%=shapes.size(); currentShape=shapes[currentShapeIndex]; std::cout << "Removed shape\n"; }
        if (key == GLFW_KEY_TAB) switchShape();

        if (key == GLFW_KEY_R) activeTransform = ROTATE;
        if (key == GLFW_KEY_T) activeTransform = TRANSLATE;
        if (key == GLFW_KEY_G) activeTransform = SCALE;

        if (key == GLFW_KEY_X) activeAxis='X';
        if (key == GLFW_KEY_Y) activeAxis='Y';
        if (key == GLFW_KEY_Z) activeAxis='Z';

        if (currentShape) {
            if (key == GLFW_KEY_KP_ADD || key == GLFW_KEY_EQUAL) {
                if(activeTransform==ROTATE) currentShape->rotate(activeAxis, +5.0f);
                if(activeTransform==TRANSLATE) currentShape->translate(activeAxis,+0.1f);
                if(activeTransform==SCALE) currentShape->scale(activeAxis,1.1f);
            }
            if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) {
                if(activeTransform==ROTATE) currentShape->rotate(activeAxis,-5.0f);
                if(activeTransform==TRANSLATE) currentShape->translate(activeAxis,-0.1f);
                if(activeTransform==SCALE) currentShape->scale(activeAxis,0.9f);
            }
        }

        if(key==GLFW_KEY_C && currentShape) {
            float r,g,b;
            std::cout<<"Enter RGB (0-1): "; std::cin>>r>>g>>b;
            currentShape->setColor(glm::vec3(r,g,b));
        }

        if(key==GLFW_KEY_S) {
            std::string fn; std::cout<<"Enter filename: "; std::cin>>fn; saveModel(fn);
        }
    }
    else if(currentMode==MODE_INSPECTION) {
        if(key==GLFW_KEY_L) { std::string fn; std::cout<<"Enter filename: "; std::cin>>fn; loadModel(fn); }
        if(key==GLFW_KEY_R) activeTransform=ROTATE;
        if(key==GLFW_KEY_X) activeAxis='X';
        if(key==GLFW_KEY_Y) activeAxis='Y';
        if(key==GLFW_KEY_Z) activeAxis='Z';

        if(activeTransform==ROTATE) {
            for(auto& s: shapes) {
                if(key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL) s->rotate(activeAxis,+5.0f);
                if(key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS) s->rotate(activeAxis,-5.0f);
            }
        }
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window=glfwCreateWindow(800,600,"Shape Modeller",NULL,NULL);
    if(!window){std::cout<<"Failed to create window\n"; glfwTerminate(); return -1;}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){std::cout<<"Failed to initialize GLAD\n"; return -1;}
    glEnable(GL_DEPTH_TEST);

    projection = glm::perspective(glm::radians(45.0f),800.0f/600.0f,0.1f,100.0f);

    while(!glfwWindowShouldClose(window)){
        moveCamera(window);

        glClearColor(0.2f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto& s: shapes) s->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
