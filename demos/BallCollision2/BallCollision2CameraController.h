#pragma once

#include "../../engine/include/rendering/camera/CameraController.h"
#include "../../engine/include/rendering/camera/CameraManager.h"
#include "../../engine/include/rendering/camera/FixedCamera.h"
#include "../../engine/include/rendering/camera/FollowCamera.h"
#include "../../engine/include/rendering/camera/OrbitCamera.h"
#include "../../engine/include/bullet/BulletRigidBody.h"

/**
 * @brief Scene-specific camera controller for BallCollision2 demo
 * 
 * Manages multiple cameras for the ball collision scene:
 * - Camera 1: Top-down overview (Fixed)
 * - Camera 2: Follow Ball 1 (Follow)
 * - Camera 3: Follow Ball 2 (Follow)
 * - Camera 4: Orbit around scene center (Orbit)
 */
class BallCollision2CameraController : public CameraController {
private:
    CameraManager m_cameraManager;
    
    // Object references
    BulletRigidBody* m_ball1;
    BulletRigidBody* m_ball2;
    BulletRigidBody* m_ground;
    
public:
    /**
     * @brief Construct the camera controller
     * @param ball1 First ball object
     * @param ball2 Second ball object
     * @param ground Ground object
     */
    BallCollision2CameraController(BulletRigidBody* ball1, BulletRigidBody* ball2, BulletRigidBody* ground);
    
    /**
     * @brief Handle input for camera switching
     * @param window GLFW window for input handling
     */
    void handleInput(GLFWwindow* window) override;
    
    /**
     * @brief Update the active camera
     * @param deltaTime Time elapsed since last frame
     */
    void update(float deltaTime) override;
    
    /**
     * @brief Get the currently active camera
     * @return Pointer to the active camera
     */
    Camera* getActiveCamera() override;
    
    /**
     * @brief Get the name of the currently active camera
     * @return Name of the active camera
     */
    const std::string& getActiveCameraName() const override;
    
private:
    /**
     * @brief Setup all cameras for the scene
     */
    void setupCameras();
    
    /**
     * @brief Handle key press events
     * @param window GLFW window
     * @param key Key code
     * @param action Key action (press/release)
     */
    void handleKeyPress(GLFWwindow* window, int key, int action);
};
