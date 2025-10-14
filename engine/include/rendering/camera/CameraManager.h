#pragma once

#include "../../../src/rendering/Camera.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

/**
 * @brief Central manager for multiple cameras in a scene
 * 
 * CameraManager handles multiple Camera instances, provides switching functionality,
 * and manages the currently active camera. It supports both direct camera selection
 * and cycling through available cameras.
 */
class CameraManager {
private:
    std::vector<std::unique_ptr<Camera>> m_cameras;
    std::vector<std::string> m_cameraNames;
    int m_activeCameraIndex = 0;
    
public:
    /**
     * @brief Add a camera to the manager
     * @param camera Unique pointer to the camera
     * @param name Display name for the camera
     * @return Index of the added camera
     */
    int addCamera(std::unique_ptr<Camera> camera, const std::string& name);
    
    /**
     * @brief Remove a camera by index
     * @param index Index of the camera to remove
     */
    void removeCamera(int index);
    
    /**
     * @brief Switch to a specific camera by index
     * @param index Index of the camera to switch to
     */
    void switchToCamera(int index);
    
    /**
     * @brief Switch to the next camera in the list
     */
    void switchToNextCamera();
    
    /**
     * @brief Switch to the previous camera in the list
     */
    void switchToPreviousCamera();
    
    /**
     * @brief Get the currently active camera
     * @return Pointer to the active camera, or nullptr if no cameras
     */
    Camera* getActiveCamera();
    
    /**
     * @brief Get the name of the currently active camera
     * @return Name of the active camera, or empty string if no cameras
     */
    const std::string& getActiveCameraName() const;
    
    /**
     * @brief Get the index of the currently active camera
     * @return Index of the active camera
     */
    int getActiveCameraIndex() const { return m_activeCameraIndex; }
    
    /**
     * @brief Get the total number of cameras
     * @return Number of cameras managed
     */
    size_t getCameraCount() const { return m_cameras.size(); }
    
    /**
     * @brief Get a camera by index
     * @param index Camera index
     * @return Pointer to camera, or nullptr if index is invalid
     */
    Camera* getCamera(size_t index) const;
    
    /**
     * @brief Update all cameras in the manager
     * @param window GLFW window for input handling
     * @param deltaTime Time elapsed since last frame
     */
    void updateAllCameras(GLFWwindow* window, float deltaTime);
    
    /**
     * @brief Check if the manager has any cameras
     * @return True if there are cameras, false otherwise
     */
    bool hasCameras() const { return !m_cameras.empty(); }
};
