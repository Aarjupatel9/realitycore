#pragma once

#include "../../../src/rendering/Camera.h"
#include "../../bullet/BulletRigidBody.h"
#include <glm/glm.hpp>

/**
 * @brief A camera that smoothly follows a target object
 * 
 * FollowCamera tracks a BulletRigidBody object with configurable offset
 * and smoothness. It provides smooth camera movement that follows the
 * target object while maintaining a fixed relative position.
 */
class FollowCamera : public Camera {
private:
    BulletRigidBody* m_targetObject;
    glm::vec3 m_offset;
    glm::vec3 m_lookAtOffset;
    float m_smoothness = 5.0f;
    
    // Current interpolated values
    glm::vec3 m_currentPosition;
    glm::vec3 m_currentLookAtTarget;
    
public:
    /**
     * @brief Construct a follow camera
     * @param target Target object to follow (must not be null)
     * @param offset Position offset relative to target
     * @param lookAtOffset Target offset for look-at point (default: target center)
     */
    FollowCamera(BulletRigidBody* target, glm::vec3 offset, glm::vec3 lookAtOffset = glm::vec3(0));
    
    /**
     * @brief Update camera position to follow target
     * @param window GLFW window (ignored for follow camera)
     * @param deltaTime Time elapsed since last frame
     */
    void update(GLFWwindow* window, float deltaTime);
    
    /**
     * @brief Set the smoothness factor
     * @param smoothness Higher values = faster following (default: 5.0)
     */
    void setSmoothness(float smoothness) { m_smoothness = smoothness; }
    
    /**
     * @brief Set the position offset relative to target
     * @param offset New position offset
     */
    void setOffset(glm::vec3 offset) { m_offset = offset; }
    
    /**
     * @brief Set the look-at offset relative to target
     * @param offset New look-at offset
     */
    void setLookAtOffset(glm::vec3 offset) { m_lookAtOffset = offset; }
    
    /**
     * @brief Get the current smoothness factor
     * @return Current smoothness value
     */
    float getSmoothness() const { return m_smoothness; }
    
    /**
     * @brief Get the position offset
     * @return Current position offset
     */
    glm::vec3 getOffset() const { return m_offset; }
    
    /**
     * @brief Get the look-at offset
     * @return Current look-at offset
     */
    glm::vec3 getLookAtOffset() const { return m_lookAtOffset; }
    
    /**
     * @brief Get the target object
     * @return Pointer to target object
     */
    BulletRigidBody* getTargetObject() const { return m_targetObject; }

private:
    /**
     * @brief Calculate yaw and pitch to look at target
     * @param position Camera position
     * @param target Target position
     */
    void calculateLookAtOrientation(glm::vec3 position, glm::vec3 target);
};
