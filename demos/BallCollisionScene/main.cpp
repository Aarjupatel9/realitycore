#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "BallCollisionScene.h"
#include "config/SceneConfig.h"

// Window dimensions
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// GLFW error callback
void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// Window resize callback
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    std::cout << "=== Ball Collision Demo ===" << std::endl;
    std::cout << "Starting Ball Collision Scene..." << std::endl;
    
    // Initialize GLFW
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Ball Collision Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Create scene configuration
    SceneConfig config;
    config.windowWidth = WINDOW_WIDTH;
    config.windowHeight = WINDOW_HEIGHT;
    
    // Create and initialize scene
    auto scene = std::make_unique<BallCollisionScene>();
    if (!scene->initialize(window, config)) {
        std::cerr << "Failed to initialize Ball Collision Scene" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << "Ball Collision Demo running successfully!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move, I/K - Up/Down, Mouse - Look" << std::endl;
    std::cout << "  Shift - Sprint, Scroll/+/- - Zoom" << std::endl;
    std::cout << "  B - Toggle controls, F - Toggle FPS display, ESC - Exit" << std::endl;
    std::cout << "  P - Start physics (balls will begin moving)" << std::endl;
    
    // Main loop
    float lastTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        // Update scene
        scene->update(deltaTime);
        
        // Render scene
        scene->render();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    scene->cleanup();
    glfwTerminate();
    
    std::cout << "Ball Collision Demo closed successfully!" << std::endl;
    return 0;
}
