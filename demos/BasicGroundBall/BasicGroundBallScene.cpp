#include "BasicGroundBallScene.h"
#include <iostream>

BasicGroundBallScene::BasicGroundBallScene() {
    std::cout << "=== BasicGroundBall Scene ===" << std::endl;
    std::cout << "Starting BasicGroundBall Scene..." << std::endl;
}

bool BasicGroundBallScene::initialize(GLFWwindow* window, const SceneConfig& config) {
    std::cout << "Initializing BasicGroundBall Scene..." << std::endl;
    
    // Call base class initialization (sets up camera, shaders, etc.)
    if (!BaseScene::initialize(window, config)) {
        return false;
    }
    
    // Note: initializeObjects() is already called by BaseScene::initialize()
    
    std::cout << "BasicGroundBall Scene initialized successfully!" << std::endl;
    return true;
}

void BasicGroundBallScene::initializeObjects() {
    std::cout << "Creating BasicGroundBall Scene objects..." << std::endl;
    
    // Create a box ground with 0.3 height, positioned so bottom is at Y=0 and top at Y=0.3
    // Box center at Y=0.15, so top surface (center + height/2) = 0.15 + 0.15 = 0.3
    // and bottom surface (center - height/2) = 0.15 - 0.15 = 0.0
    createBox(glm::vec3(0.0f, 0.15f, 0.0f), glm::vec3(10.0f, 0.3f, 10.0f), 
              glm::vec3(0.0f), glm::vec3(0.3f, 0.3f, 0.3f), true, 0.0f); // Mass = 0 for static
    
    // Create a ball at height 2.0 with radius 0.5
    // Expected final position: Y = 0.3 (ground top) + 0.5 (radius) + 0.01 (small gap) = 0.81
    createSphere(glm::vec3(0.0f, 2.0f, 0.0f), 0.5f, 
                 glm::vec3(0.0f, 1.0f, 0.0f), // Green color
                 true, 1.0f); // Physics enabled, mass = 1.0f
    
    std::cout << "DEBUG: Ground box created at Y=0.15 with height 0.3 (bottom at Y=0.0, top at Y=0.3)" << std::endl;
    std::cout << "DEBUG: Ball created at Y=2.0 with radius 0.5 (expected final Y=0.8)" << std::endl;
    std::cout << "DEBUG: Total objects created: " << m_objects.size() << std::endl;
    std::cout << "DEBUG: Total physics objects: " << m_physicsObjects.size() << std::endl;
}

void BasicGroundBallScene::update(float deltaTime) {
    // Call base class update (handles physics, camera, etc.)
    BaseScene::update(deltaTime);
    
    // Add your custom update logic here
}

void BasicGroundBallScene::render() {
    // Call base class render (handles all rendering)
    BaseScene::render();
}

void BasicGroundBallScene::cleanup() {
    std::cout << "Cleaning up BasicGroundBall Scene..." << std::endl;
    BaseScene::cleanup();
    std::cout << "BasicGroundBall Scene cleanup complete" << std::endl;
}
