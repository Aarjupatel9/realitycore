#include "BallFreeFallScene.h"
#include <iostream>

BallFreeFallScene::BallFreeFallScene() {
    std::cout << "=== BallFreeFall Scene ===" << std::endl;
    std::cout << "Starting BallFreeFall Scene..." << std::endl;
}

bool BallFreeFallScene::initialize(GLFWwindow* window, const SceneConfig& config) {
    std::cout << "Initializing BallFreeFall Scene..." << std::endl;
    
    // Call base class initialization (sets up camera, shaders, etc.)
    if (!BaseScene::initialize(window, config)) {
        return false;
    }
    
    // Note: initializeObjects() is already called by BaseScene::initialize()
    
    std::cout << "BallFreeFall Scene initialized successfully!" << std::endl;
    return true;
}

void BallFreeFallScene::initializeObjects() {
    std::cout << "Creating BallFreeFall Scene objects..." << std::endl;
    
    // Create a ball falling under gravity (dynamic)
    createSphere(glm::vec3(0.0f, 5.0f, 0.0f), 0.5f, 
                 glm::vec3(1.0f, 0.0f, 0.0f), // Red color
                 true, 1.0f); // Physics enabled, mass = 1.0f
    
    std::cout << "BallFreeFall Scene: Created 1 ball falling under gravity" << std::endl;
}

void BallFreeFallScene::update(float deltaTime) {
    // Call base class update (handles physics, camera, etc.)
    BaseScene::update(deltaTime);
    
    // Add your custom update logic here
}

void BallFreeFallScene::render() {
    // Call base class render (handles all rendering)
    BaseScene::render();
}

void BallFreeFallScene::cleanup() {
    std::cout << "Cleaning up BallFreeFall Scene..." << std::endl;
    BaseScene::cleanup();
    std::cout << "BallFreeFall Scene cleanup complete" << std::endl;
}
