#include "../../../include/rendering/camera/OrbitCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

OrbitCamera::OrbitCamera(BulletRigidBody* center, float radius, float height, float speed)
    : m_centerObject(center), m_radius(radius), m_height(height), m_orbitSpeed(speed) {
    
    setControlsEnabled(false); // Disable input controls for orbit camera
    
    // Initialize camera at starting position
    update(nullptr, 0.0f);
}

void OrbitCamera::update(GLFWwindow* window, float deltaTime) {
    // Update orbit angle
    m_currentAngle += m_orbitSpeed * deltaTime;
    
    // Keep angle in [0, 2π] range
    while (m_currentAngle >= 2.0f * glm::pi<float>()) {
        m_currentAngle -= 2.0f * glm::pi<float>();
    }
    while (m_currentAngle < 0.0f) {
        m_currentAngle += 2.0f * glm::pi<float>();
    }
    
    // Calculate center position
    glm::vec3 centerPos(0.0f);
    if (m_centerObject) {
        centerPos = m_centerObject->getPosition();
    }
    
    // Calculate camera position on orbit
    float x = centerPos.x + m_radius * cos(m_currentAngle);
    float z = centerPos.z + m_radius * sin(m_currentAngle);
    float y = centerPos.y + m_height;
    
    glm::vec3 cameraPos(x, y, z);
    
    // Update camera position and look at center
    setPosition(cameraPos);
    calculateLookAtCenter(cameraPos, centerPos);
    
    // Don't call base class update to avoid input processing
}

void OrbitCamera::calculateLookAtCenter(glm::vec3 position, glm::vec3 center) {
    glm::vec3 direction = glm::normalize(center - position);
    
    // Calculate yaw (rotation around Y axis)
    float yaw = glm::degrees(atan2(direction.z, direction.x));
    
    // Calculate pitch (rotation around X axis)
    float pitch = glm::degrees(asin(direction.y));
    
    setYaw(yaw);
    setPitch(pitch);
}
