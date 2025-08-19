<!
std::string promptForInput(const std::string& prompt_message) {
    std::cout << prompt_message << std::flush;

    // Clear potential error states and discard any characters left in the buffer,
    // especially the newline character from the key press that triggered this prompt.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::string line;
    std::getline(std::cin, line);
    
    return line;
}

// --- GLFW Callback Forwarding ---
// GLFW callbacks must be static or global functions. To allow them to call methods
// on our Application instance, we use a standard C-style callback pattern.
// We store a pointer to the Application instance in the GLFW window's user pointer
// and retrieve it in the static callback, which then forwards the call.
static void key_callback_dispatch(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->onKey(key, scancode, action, mods);
    }
}

// --- Application Class Implementation ---

Application::Application() {
    // Initialize with a default empty model
    model = std::make_unique<model_t>();
}

Application::~Application() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void Application::init() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Request OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create the window
    window = glfwCreateWindow(1280, 720, "Hierarchical 3D Modeller", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit()!= GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }

    // Set up callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback_dispatch);

    // Initialize subsystems
    renderer = std::make_unique<Renderer>();
    camera = std::make_unique<Camera>(glm::vec3(0.0f, 2.0f, 10.0f));
    shader = std::make_unique<Shader>("shaders/simple.vert", "shaders/simple.frag");

    // Initial OpenGL state
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    std::cout << "Application Initialized. Starting in Modelling Mode." << std::endl;
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        // Event handling
        glfwPollEvents();

        // Rendering
        renderer->clear();
        shader->use();

        // Set camera uniforms
        shader->setMat4("projection", camera->getProjectionMatrix());
        shader->setMat4("view", camera->getViewMatrix());

        // Draw the model
        if (model) {
            renderer->drawModel(*model, *shader);
        }

        // Swap buffers
        glfwSwapBuffers(window);
    }
}

void Application::onKey(int key, int scancode, int action, int mods) {
    if (action!= GLFW_PRESS && action!= GLFW_REPEAT) {
        return;
    }

    // --- Global Keys ---
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    if (key == GLFW_KEY_M) {
        currentMode = AppMode::MODELLING;
        resetTransformState();
        std::cout << "\n--- Switched to MODELLING Mode ---" << std::endl;
        return;
    }
    if (key == GLFW_KEY_I) {
        currentMode = AppMode::INSPECTION;
        resetTransformState();
        std::cout << "\n--- Switched to INSPECTION Mode ---" << std::endl;
        return;
    }

    // --- Mode-Specific Keys (Finite State Machine) ---
    switch (currentMode) {
        case AppMode::MODELLING:
            handleModellingInput(key);
            break;
        case AppMode::INSPECTION:
            handleInspectionInput(key);
            break;
    }
}

void Application::handleModellingInput(int key) {
    // --- Shape Creation / Deletion ---
    if (currentTransformMode == TransformMode::NONE) {
        switch (key) {
            case GLFW_KEY_1: addShape(ShapeType::SPHERE_SHAPE); break;
            case GLFW_KEY_2: addShape(ShapeType::CYLINDER_SHAPE); break;
            case GLFW_KEY_3: addShape(ShapeType::BOX_SHAPE); break;
            case GLFW_KEY_4: addShape(ShapeType::CONE_SHAPE); break;
            case GLFW_KEY_5: removeLastShape(); break;
            case GLFW_KEY_T: currentTransformMode = TransformMode::TRANSLATE; std::cout << "Activated TRANSLATE mode." << std::endl; break;
            case GLFW_KEY_R: currentTransformMode = TransformMode::ROTATE; std::cout << "Activated ROTATE mode." << std::endl; break;
            case GLFW_KEY_G: currentTransformMode = TransformMode::SCALE; std::cout << "Activated SCALE mode." << std::endl; break;
            case GLFW_KEY_S: saveModel(); break;
            case GLFW_KEY_C: changeCurrentShapeColor(); break;
        }
    }
    // --- Transformation Sub-Modes ---
    else {
        handleTransformationInput(key, model->getCurrentNode());
    }
}

void Application::handleInspectionInput(int key) {
    if (currentTransformMode == TransformMode::NONE) {
        switch (key) {
            case GLFW_KEY_L: loadModel(); break;
            case GLFW_KEY_R: currentTransformMode = TransformMode::ROTATE; std::cout << "Activated ROTATE mode for entire model." << std::endl; break;
        }
    }
    // --- Transformation Sub-Modes ---
    else {
        handleTransformationInput(key, model->getRootNode());
    }
}

void Application::handleTransformationInput(int key, HNode* targetNode) {
    if (!targetNode) {
        std::cout << "No shape selected to transform." << std::endl;
        resetTransformState();
        return;
    }

    // --- Axis Selection ---
    if (currentAxis == TransformAxis::NONE) {
        switch (key) {
            case GLFW_KEY_X: currentAxis = TransformAxis::X; std::cout << "Selected X-axis." << std::endl; break;
            case GLFW_KEY_Y: currentAxis = TransformAxis::Y; std::cout << "Selected Y-axis." << std::endl; break;
            case GLFW_KEY_Z: currentAxis = TransformAxis::Z; std::cout << "Selected Z-axis." << std::endl; break;
            default: resetTransformState(); break; // Any other key exits transform mode
        }
    }
    // --- Value Adjustment ---
    else {
        float increment = (key == GLFW_KEY_EQUAL |

| key == GLFW_KEY_KP_ADD)? 1.0f : -1.0f;
        if (key!= GLFW_KEY_EQUAL && key!= GLFW_KEY_KP_ADD && key!= GLFW_KEY_MINUS && key!= GLFW_KEY_KP_SUBTRACT) {
            resetTransformState();
            return;
        }

        glm::vec3 axisVec(
            (currentAxis == TransformAxis::X)? 1.0f : 0.0f,
            (currentAxis == TransformAxis::Y)? 1.0f : 0.0f,
            (currentAxis == TransformAxis::Z)? 1.0f : 0.0f
        );

        switch (currentTransformMode) {
            case TransformMode::TRANSLATE:
                targetNode->translate(axisVec * 0.1f * increment);
                break;
            case TransformMode::ROTATE:
                targetNode->rotate(glm::radians(5.0f * increment), axisVec);
                break;
            case TransformMode::SCALE:
                targetNode->scale(glm::vec3(1.0) + (axisVec * 0.1f * increment));
                break;
            case TransformMode::NONE: break;
        }
    }
}

void Application::addShape(ShapeType type) {
    std::unique_ptr<shape_t> newShape;
    unsigned int defaultTessellation = 2; // A reasonable default

    switch (type) {
        case ShapeType::SPHERE_SHAPE:
            newShape = std::make_unique<sphere_t>(defaultTessellation);
            std::cout << "Added Sphere." << std::endl;
            break;
        case ShapeType::CYLINDER_SHAPE:
            newShape = std::make_unique<cylinder_t>(defaultTessellation);
            std::cout << "Added Cylinder." << std::endl;
            break;
        case ShapeType::BOX_SHAPE:
            newShape = std::make_unique<box_t>(defaultTessellation);
            std::cout << "Added Box." << std::endl;
            break;
        case ShapeType::CONE_SHAPE:
            newShape = std::make_unique<cone_t>(defaultTessellation);
            std::cout << "Added Cone." << std::endl;
            break;
    }

    if (newShape) {
        model->addShape(std::move(newShape));
    }
}

void Application::removeLastShape() {
    if (model->removeLastShape()) {
        std::cout << "Removed last shape." << std::endl;
    } else {
        std::cout << "No shapes to remove." << std::endl;
    }
}

void Application::changeCurrentShapeColor() {
    HNode* currentNode = model->getCurrentNode();
    if (!currentNode ||!currentNode->shape) {
        std::cout << "No shape selected to color." << std::endl;
        return;
    }

    std::string input = promptForInput("Enter new color (R G B, values 0.0-1.0): ");
    std::stringstream ss(input);
    float r, g, b;
    if (ss >> r >> g >> b) {
        currentNode->shape->setColor(glm::vec4(r, g, b, 1.0f));
        std::cout << "Color updated." << std::endl;
    } else {
        std::cout << "Invalid color format." << std::endl;
    }
}

void Application::saveModel() {
    std::string filename = promptForInput("Enter filename to save (e.g., my_model.mod): ");
    if (filename.empty()) {
        std::cout << "Save cancelled." << std::endl;
        return;
    }
    if (model->saveToFile(filename)) {
        std::cout << "Model saved to " << filename << std::endl;
    } else {
        std::cout << "Failed to save model to " << filename << std::endl;
    }
}

void Application::loadModel() {
    std::string filename = promptForInput("Enter filename to load: ");
    if (filename.empty()) {
        std::cout << "Load cancelled." << std::endl;
        return;
    }

    auto newModel = std::make_unique<model_t>();
    if (newModel->loadFromFile(filename)) {
        model = std::move(newModel);
        std::cout << "Model loaded from " << filename << std::endl;
        // Frame the newly loaded model so it's visible
        camera->frameModel(*model);
    } else {
        std::cout << "Failed to load model from " << filename << std::endl;
    }
}

void Application::resetTransformState() {
    currentTransformMode = TransformMode::NONE;
    currentAxis = TransformAxis::NONE;
    std::cout << "Exited transformation mode." << std::endl;
}
```
]]>
