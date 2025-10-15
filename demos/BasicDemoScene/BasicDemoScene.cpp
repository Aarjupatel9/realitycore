#include "BasicDemoScene.h"
#include <iostream>

BasicDemoScene::BasicDemoScene() {
    // Constructor - BaseScene handles most initialization
}

bool BasicDemoScene::initialize(GLFWwindow* window, const SceneConfig& config) {
    // Call parent initialize to setup common components
    if (!BaseScene::initialize(window, config)) {
        return false;
    }
    
    // Initialize scene-specific components
    initializeCamera();
    
    std::cout << "Basic Demo Scene initialized successfully!" << std::endl;
    return true;
}

void BasicDemoScene::initializeCamera() {
    // Set camera position for better viewing
    if (m_performanceTestMode) {
        m_camera->setPosition(glm::vec3(0.0f, 15.0f, 25.0f)); // Higher and further back for performance test
        m_camera->setYaw(-90.0f); // Look towards center
        m_camera->setPitch(-20.0f); // Look down slightly
    } else {
        m_camera->setPosition(glm::vec3(0.0f, 8.0f, 12.0f)); // Better distance for basic demo
        m_camera->setYaw(-90.0f); // Look towards center
        m_camera->setPitch(-15.0f); // Look down to see ground
    }
}

void BasicDemoScene::initializeObjects() {
    std::cout << "Creating Basic Demo objects..." << std::endl;
    
    // Create ground plane
    createPlane(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(20.0f, 20.0f), 
                glm::vec3(0.0f), glm::vec3(0.7f, 0.7f, 0.7f), false); // Static ground
    
    if (m_performanceTestMode) {
        // Create many objects for performance testing
        createPerformanceTestObjects();
    } else {
        // Create basic demo objects
        
        // Create cube
        createBox(glm::vec3(-1.0f, 2.0f, 0.0f), 
                  glm::vec3(1.0f, 1.0f, 1.0f), 
                  glm::vec3(0.0f), glm::vec3(0.8f, 0.2f, 0.2f), true, 10.0f); // Red cube with physics
        
        // Create sphere
        createSphere(glm::vec3(1.0f, 3.0f, 0.0f), 0.5f, 
                     glm::vec3(0.2f, 0.2f, 0.8f), true, 5.0f); // Blue sphere with physics
    }
    
    std::cout << "Created basic demo objects" << std::endl;
}

void BasicDemoScene::createPerformanceTestObjects() {
    std::cout << "Creating performance test objects..." << std::endl;
    
    // Create many cubes and spheres for performance testing
    for (int i = 0; i < 100; i++) {
        float x = (i % 10) * 2.0f - 9.0f;
        float z = (i / 10) * 2.0f - 9.0f;
        float y = 5.0f + (i % 3) * 2.0f;
        
        if (i % 2 == 0) {
            // Create cube
            createBox(glm::vec3(x, y, z), 
                      glm::vec3(0.5f, 0.5f, 0.5f), 
                      glm::vec3(0.0f), glm::vec3(0.8f, 0.2f, 0.2f), true, 1.0f);
        } else {
            // Create sphere
            createSphere(glm::vec3(x, y, z), 0.25f, 
                         glm::vec3(0.2f, 0.2f, 0.8f), true, 0.5f);
        }
    }
    
    std::cout << "Created 100 performance test objects" << std::endl;
}

void BasicDemoScene::update(float deltaTime) {
    // BaseScene handles common updates
    BaseScene::update(deltaTime);
    updateScene(deltaTime);
}

void BasicDemoScene::render() {
    // BaseScene handles common rendering
    BaseScene::render();
    renderScene();
}

void BasicDemoScene::updateScene(float deltaTime) {
    // Scene-specific update logic can be added here if needed
}

void BasicDemoScene::renderScene() {
    // Scene-specific rendering can be added here if needed
}

void BasicDemoScene::cleanup() {
    // BaseScene handles cleanup
    std::cout << "Cleaning up Basic Demo Scene..." << std::endl;
}
