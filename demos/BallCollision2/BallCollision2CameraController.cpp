#include "BallCollision2CameraController.h"
#include <GLFW/glfw3.h>
#include <iostream>

BallCollision2CameraController::BallCollision2CameraController(BulletRigidBody* ball1, BulletRigidBody* ball2, BulletRigidBody* ground)
    : m_ball1(ball1), m_ball2(ball2), m_ground(ground) {
    
    if (!m_ball1 || !m_ball2 || !m_ground) {
        std::cerr << "Warning: BallCollision2CameraController created with null object references" << std::endl;
    }
    
    setupCameras();
    
    std::cout << "🎥 BallCollision2 Camera System initialized with " << m_cameraManager.getCameraCount() << " cameras" << std::endl;
    std::cout << "📋 Camera Controls:" << std::endl;
    std::cout << "   1 - Free-fly Camera (WASD movement)" << std::endl;
    std::cout << "   2 - Top-down Overview" << std::endl;
    std::cout << "   3 - Follow Ball 1 (Red)" << std::endl;
    std::cout << "   4 - Follow Ball 2 (Green)" << std::endl;
    std::cout << "   5 - Orbit Around Scene" << std::endl;
    std::cout << "   Tab - Next Camera" << std::endl;
    std::cout << "   Shift+Tab - Previous Camera" << std::endl;
}

void BallCollision2CameraController::handleInput(GLFWwindow* window) {
    // Check for camera switching keys
    static bool key1Pressed = false;
    static bool key2Pressed = false;
    static bool key3Pressed = false;
    static bool key4Pressed = false;
    static bool key5Pressed = false;
    static bool tabPressed = false;
    static bool shiftTabPressed = false;
    
    // Safety check: don't process input if window is NULL
    if (!window) {
        return;
    }
    
    // Direct camera selection
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !key1Pressed) {
        std::cout << "🎥 Switching to Camera 1: Free-fly Camera" << std::endl;
        m_cameraManager.switchToCamera(0);
        key1Pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        key1Pressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !key2Pressed) {
        std::cout << "🎥 Switching to Camera 2: Top-down Overview" << std::endl;
        m_cameraManager.switchToCamera(1);
        key2Pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        key2Pressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !key3Pressed) {
        std::cout << "🎥 Switching to Camera 3: Follow Ball 1 (Red)" << std::endl;
        m_cameraManager.switchToCamera(2);
        key3Pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        key3Pressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !key4Pressed) {
        std::cout << "🎥 Switching to Camera 4: Follow Ball 2 (Green)" << std::endl;
        m_cameraManager.switchToCamera(3);
        key4Pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
        key4Pressed = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !key5Pressed) {
        std::cout << "🎥 Switching to Camera 5: Orbit Around Scene" << std::endl;
        m_cameraManager.switchToCamera(4);
        key5Pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) {
        key5Pressed = false;
    }
    
    // Camera cycling
    bool shiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
                        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (shiftPressed && !shiftTabPressed) {
            std::cout << "🎥 Switching to Previous Camera" << std::endl;
            m_cameraManager.switchToPreviousCamera();
            shiftTabPressed = true;
        } else if (!shiftPressed && !tabPressed) {
            std::cout << "🎥 Switching to Next Camera" << std::endl;
            m_cameraManager.switchToNextCamera();
            tabPressed = true;
        }
    } else {
        tabPressed = false;
        shiftTabPressed = false;
    }
}

void BallCollision2CameraController::update(float deltaTime) {
    // Update ALL cameras to ensure follow cameras track their targets
    // even when not the active camera
    for (size_t i = 0; i < m_cameraManager.getCameraCount(); i++) {
        Camera* camera = m_cameraManager.getCamera(i);
        if (camera) {
            // Use dynamic_cast to check if it's a FollowCamera
            FollowCamera* followCamera = dynamic_cast<FollowCamera*>(camera);
            if (followCamera) {
                // Update follow camera with its own method
                followCamera->update(nullptr, deltaTime);
            } else {
                // For other camera types, use base update method
                camera->update(nullptr, deltaTime);
            }
        }
    }
}

Camera* BallCollision2CameraController::getActiveCamera() {
    return m_cameraManager.getActiveCamera();
}

const std::string& BallCollision2CameraController::getActiveCameraName() const {
    return m_cameraManager.getActiveCameraName();
}

void BallCollision2CameraController::setupCameras() {
    // Camera 1: Free-fly camera (movable with WASD)
    auto freeFlyCamera = std::make_unique<Camera>();
    freeFlyCamera->setPosition(glm::vec3(0.0f, 5.0f, 10.0f));
    m_cameraManager.addCamera(std::move(freeFlyCamera), "Free-fly Camera");
    
    // Camera 2: Top-down overview (Fixed)
    auto overviewCamera = std::make_unique<FixedCamera>(
        glm::vec3(0.0f, 8.0f, 0.0f),  // Position: high above center
        glm::vec3(0.0f, 0.0f, 0.0f)   // Target: scene center
    );
    m_cameraManager.addCamera(std::move(overviewCamera), "Top-down Overview");
    
    // Camera 3: Follow Ball 1 (Follow)
    if (m_ball1) {
        std::cout << "Creating Follow Ball 1 camera with ball at position: " 
                  << m_ball1->getPosition().x << ", " << m_ball1->getPosition().y << ", " << m_ball1->getPosition().z << std::endl;
        auto followBall1Camera = std::make_unique<FollowCamera>(
            m_ball1,                          // Target: Ball 1
            glm::vec3(-2.0f, 1.5f, -2.0f),   // Offset: behind and above
            glm::vec3(0.0f, 0.0f, 0.0f)      // Look at ball center
        );
        followBall1Camera->setSmoothness(3.0f);
        m_cameraManager.addCamera(std::move(followBall1Camera), "Follow Ball 1 (Red)");
    } else {
        std::cout << "Warning: Ball 1 reference is null, cannot create follow camera" << std::endl;
    }
    
    // Camera 4: Follow Ball 2 (Follow)
    if (m_ball2) {
        std::cout << "Creating Follow Ball 2 camera with ball at position: " 
                  << m_ball2->getPosition().x << ", " << m_ball2->getPosition().y << ", " << m_ball2->getPosition().z << std::endl;
        auto followBall2Camera = std::make_unique<FollowCamera>(
            m_ball2,                          // Target: Ball 2
            glm::vec3(2.0f, 1.5f, 2.0f),     // Offset: different angle
            glm::vec3(0.0f, 0.0f, 0.0f)      // Look at ball center
        );
        followBall2Camera->setSmoothness(3.0f);
        m_cameraManager.addCamera(std::move(followBall2Camera), "Follow Ball 2 (Green)");
    } else {
        std::cout << "Warning: Ball 2 reference is null, cannot create follow camera" << std::endl;
    }
    
    // Camera 5: Orbit around scene center (Orbit)
    auto orbitCamera = std::make_unique<OrbitCamera>(
        nullptr,    // Center: world origin
        6.0f,       // Radius: 6 meters
        3.0f,       // Height: 3 meters above center
        0.5f        // Speed: 0.5 radians per second
    );
    m_cameraManager.addCamera(std::move(orbitCamera), "Orbit Around Scene");
}
