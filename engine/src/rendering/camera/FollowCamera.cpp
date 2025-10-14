#include "../../../include/rendering/camera/FollowCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

FollowCamera::FollowCamera(BulletRigidBody* target, glm::vec3 offset, glm::vec3 lookAtOffset)
    : m_targetObject(target), m_offset(offset), m_lookAtOffset(lookAtOffset) {
    
    if (!m_targetObject) {
        std::cerr << "Warning: FollowCamera created with null target object" << std::endl;
        return;
    }
    
    // Initialize current position based on target's initial position
    glm::vec3 targetPos = m_targetObject->getPosition();
    m_currentPosition = targetPos + m_offset;
    m_currentLookAtTarget = targetPos + m_lookAtOffset;
    
    // Set initial camera state
    setPosition(m_currentPosition);
    calculateLookAtOrientation(m_currentPosition, m_currentLookAtTarget);
    setControlsEnabled(false); // Disable input controls for follow camera
}

void FollowCamera::update(GLFWwindow* window, float deltaTime) {
    if (!m_targetObject) {
        return;
    }
    
    // Get target's current position
    glm::vec3 targetPos = m_targetObject->getPosition();
    
    // Calculate desired camera position and target
    glm::vec3 desiredPosition = targetPos + m_offset;
    glm::vec3 desiredLookAtTarget = targetPos + m_lookAtOffset;
    
    // Smoothly interpolate to desired position
    float lerpFactor = 1.0f - exp(-m_smoothness * deltaTime);
    
    m_currentPosition = glm::mix(m_currentPosition, desiredPosition, lerpFactor);
    m_currentLookAtTarget = glm::mix(m_currentLookAtTarget, desiredLookAtTarget, lerpFactor);
    
    // Update camera state
    setPosition(m_currentPosition);
    calculateLookAtOrientation(m_currentPosition, m_currentLookAtTarget);
    
    // Don't call base class update to avoid input processing and glfwGetKey calls
    // Follow cameras don't need input handling
}

void FollowCamera::calculateLookAtOrientation(glm::vec3 position, glm::vec3 target) {
    glm::vec3 direction = glm::normalize(target - position);
    
    // Calculate yaw (rotation around Y axis)
    float yaw = glm::degrees(atan2(direction.z, direction.x));
    
    // Calculate pitch (rotation around X axis)
    float pitch = glm::degrees(asin(direction.y));
    
    setYaw(yaw);
    setPitch(pitch);
}
