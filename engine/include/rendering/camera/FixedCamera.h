#pragma once

#include "../../../src/rendering/Camera.h"
#include <glm/glm.hpp>

/**
 * @brief A camera with fixed position and orientation
 * 
 * FixedCamera maintains a static position and looks in a fixed direction.
 * It does not move or track objects, making it ideal for overview shots
 * or static viewpoints.
 */
class FixedCamera : public Camera {
private:
    glm::vec3 m_fixedPosition;
    float m_fixedYaw;
    float m_fixedPitch;
    
public:
    /**
     * @brief Construct a fixed camera
     * @param position Camera position in world space
     * @param target Target point to look at
     */
    FixedCamera(glm::vec3 position, glm::vec3 target);
    
    /**
     * @brief Construct a fixed camera with yaw/pitch
     * @param position Camera position in world space
     * @param yaw Yaw angle in degrees
     * @param pitch Pitch angle in degrees
     */
    FixedCamera(glm::vec3 position, float yaw, float pitch);
    
    /**
     * @brief Update camera (overrides input handling)
     * @param window GLFW window (ignored for fixed camera)
     * @param deltaTime Time elapsed since last frame
     */
    void update(GLFWwindow* window, float deltaTime);
    
    /**
     * @brief Set the fixed position
     * @param position New camera position
     */
    void setFixedPosition(glm::vec3 position);
    
    /**
     * @brief Set the fixed target (calculates yaw/pitch)
     * @param target New target position
     */
    void setFixedTarget(glm::vec3 target);
    
    /**
     * @brief Set the fixed orientation
     * @param yaw Yaw angle in degrees
     * @param pitch Pitch angle in degrees
     */
    void setFixedOrientation(float yaw, float pitch);
    
    /**
     * @brief Get the fixed position
     * @return Current fixed position
     */
    glm::vec3 getFixedPosition() const { return m_fixedPosition; }
    
    /**
     * @brief Get the fixed yaw
     * @return Current fixed yaw in degrees
     */
    float getFixedYaw() const { return m_fixedYaw; }
    
    /**
     * @brief Get the fixed pitch
     * @return Current fixed pitch in degrees
     */
    float getFixedPitch() const { return m_fixedPitch; }

private:
    /**
     * @brief Calculate yaw and pitch from position and target
     * @param position Camera position
     * @param target Target position
     */
    void calculateOrientation(glm::vec3 position, glm::vec3 target);
};
