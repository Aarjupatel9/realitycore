#pragma once

#include <glm/glm.hpp>

/**
 * CameraConfig - Camera system configuration
 * 
 * Controls initial camera state, movement parameters, and FOV limits.
 * Defaults provide a standard free-fly camera experience.
 */
struct CameraConfig {
    // Initial state
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);          // Start position
    float yaw = -90.0f;                                         // Looking forward (-Z axis)
    float pitch = 0.0f;                                         // Level (horizontal)
    
    // View parameters
    float fov = 45.0f;                                          // Field of view in degrees
    float nearPlane = 0.1f;                                     // Near clipping plane
    float farPlane = 100.0f;                                    // Far clipping plane
    
    // Movement
    float moveSpeed = 3.0f;                                     // Base movement speed (m/s)
    float sprintMultiplier = 2.5f;                              // Sprint speed boost (shift key)
    float mouseSensitivity = 0.1f;                              // Mouse look sensitivity
    
    // FOV limits (for zoom with +/- keys)
    float minFOV = 20.0f;                                       // Minimum FOV (zoomed in)
    float maxFOV = 90.0f;                                       // Maximum FOV (zoomed out)
    
    // Controls
    bool controlsEnabled = true;                                 // Enable/disable input
};

