#include "../../../include/rendering/camera/FixedCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

FixedCamera::FixedCamera(glm::vec3 position, glm::vec3 target)
    : m_fixedPosition(position) {
    
    calculateOrientation(position, target);
    
    // Initialize the camera with the fixed values
    setPosition(position);
    setYaw(m_fixedYaw);
    setPitch(m_fixedPitch);
    setControlsEnabled(false); // Disable input controls for fixed camera
}

FixedCamera::FixedCamera(glm::vec3 position, float yaw, float pitch)
    : m_fixedPosition(position), m_fixedYaw(yaw), m_fixedPitch(pitch) {
    
    // Initialize the camera with the fixed values
    setPosition(position);
    setYaw(yaw);
    setPitch(pitch);
    setControlsEnabled(false); // Disable input controls for fixed camera
}

void FixedCamera::update(GLFWwindow* window, float deltaTime) {
    // Fixed cameras don't respond to input, but we maintain the fixed state
    setPosition(m_fixedPosition);
    setYaw(m_fixedYaw);
    setPitch(m_fixedPitch);
    
    // Don't call base class update to avoid input processing
}

void FixedCamera::setFixedPosition(glm::vec3 position) {
    m_fixedPosition = position;
    setPosition(position);
}

void FixedCamera::setFixedTarget(glm::vec3 target) {
    calculateOrientation(m_fixedPosition, target);
    setYaw(m_fixedYaw);
    setPitch(m_fixedPitch);
}

void FixedCamera::setFixedOrientation(float yaw, float pitch) {
    m_fixedYaw = yaw;
    m_fixedPitch = pitch;
    setYaw(yaw);
    setPitch(pitch);
}

void FixedCamera::calculateOrientation(glm::vec3 position, glm::vec3 target) {
    glm::vec3 direction = glm::normalize(target - position);
    
    // Calculate yaw (rotation around Y axis)
    m_fixedYaw = glm::degrees(atan2(direction.z, direction.x));
    
    // Calculate pitch (rotation around X axis)
    m_fixedPitch = glm::degrees(asin(direction.y));
}
