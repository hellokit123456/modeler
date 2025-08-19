/**
 * @file main.cpp
 * @brief The entry point for the 3D Hierarchical Modeller application.
 *
 * This file contains the main function, which is the starting point of the program.
 * Its primary responsibilities are to create an instance of the Application class,
 * initialize it, and then execute its main loop. All application-specific logic,
 * including window creation, event handling, and rendering, is encapsulated
 * within the Application class to keep the main function clean and simple.
 */

#include "application.hpp" 
#include <iostream>        
#include <stdexcept>       

int main() {
    // Using std::unique_ptr is a good practice for managing the lifetime of the
    // application object, ensuring its destructor is called automatically upon exit,
    // which handles resource cleanup (e.g., terminating GLFW).
    auto app = std::make_unique<Application>();

    try {
        // Initialize the application. This sets up GLFW, creates the window,
        // initializes GLEW, loads shaders, and sets up initial OpenGL state.
        app->init();

        // the user closes the window or presses the Escape key. It handles
        // polling for events, rendering the scene, and swapping buffers.
        app->run();

    } catch (const std::runtime_error& e) {
        // Catch any standard runtime errors that might be thrown during
        std::cerr << "A critical runtime error occurred: " << e.what() << std::endl;
        return EXIT_FAILURE; // Return a failure code
    } catch (...) {
        // A catch-all for any other unexpected exceptions.
        std::cerr << "An unexpected error occurred." << std::endl;
        return EXIT_FAILURE; // Return a failure code
    }

    // If the application loop exits cleanly, return a success code.
    return EXIT_SUCCESS;
}
