#pragma once

#include "../../../src/rendering/Camera.h"
#include <string>

// Forward declaration
struct GLFWwindow;

/**
 * @brief Abstract base class for camera controllers
 * 
 * CameraController defines the interface for managing camera behavior in scenes.
 * Concrete implementations can provide different camera types like Fixed, Follow,
 * Attached, or Orbit cameras.
 */
class CameraController {
public:
    virtual ~CameraController() = default;
    
    /**
     * @brief Handle input for camera control
     * @param window GLFW window for input handling
     */
    virtual void handleInput(GLFWwindow* window) = 0;
    
    /**
     * @brief Update camera state
     * @param deltaTime Time elapsed since last frame
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * @brief Get the currently active camera
     * @return Pointer to the active camera
     */
    virtual Camera* getActiveCamera() = 0;
    
    /**
     * @brief Get the name of the currently active camera
     * @return Name of the active camera
     */
    virtual const std::string& getActiveCameraName() const = 0;
    
    /**
     * @brief Called when this controller becomes active
     */
    virtual void onActivate() {}
    
    /**
     * @brief Called when this controller becomes inactive
     */
    virtual void onDeactivate() {}
};
