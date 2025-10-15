#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "BasicGroundBallScene.h"
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
    std::cout << "=== BasicGroundBall Demo ===" << std::endl;
    std::cout << "Starting BasicGroundBall Scene..." << std::endl;
    
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BasicGroundBall Scene", nullptr, nullptr);
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
    
    try {
        // Create scene configuration
        SceneConfig config;
        config.windowWidth = WINDOW_WIDTH;
        config.windowHeight = WINDOW_HEIGHT;
        
        BasicGroundBallScene scene;
        
        if (!scene.initialize(window, config)) {
            std::cerr << "Failed to initialize BasicGroundBall Scene" << std::endl;
            glfwTerminate();
            return -1;
        }
        
        std::cout << "BasicGroundBall Demo running successfully!" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Move, I/K - Up/Down, Mouse - Look" << std::endl;
        std::cout << "  Shift - Sprint, Scroll/+/- - Zoom" << std::endl;
        std::cout << "  B - Toggle controls, F - Toggle FPS display, ESC - Exit" << std::endl;
        
        // Main loop
        while (!glfwWindowShouldClose(window)) {
            float currentTime = glfwGetTime();
            static float lastTime = currentTime;
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;
            
            // Update scene
            scene.update(deltaTime);
            
            // Render scene
            scene.render();
            
            // Swap buffers and poll events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        scene.cleanup();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwTerminate();
    std::cout << "BasicGroundBall Demo closed successfully!" << std::endl;
    return 0;
}
