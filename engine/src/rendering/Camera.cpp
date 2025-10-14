#include "Camera.h"
#include <iostream>

Camera* Camera::s_instance = nullptr;

Camera::Camera() 
    : m_position(0.0f, 0.0f, 5.0f)
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
    , m_fov(45.0f)
    , m_moveSpeed(3.0f)
    , m_mouseSensitivity(0.1f)
    , m_controlsEnabled(true)
    , m_firstMouse(true)
    , m_fpsToggleRequested(false)
    , m_lastMouseX(400.0)
    , m_lastMouseY(300.0)
{
    s_instance = this;
}

void Camera::update(GLFWwindow* window, float deltaTime) {
    if (!m_controlsEnabled) return;
    if (!window) return; // Don't process input if window is NULL
    
    glm::vec3 front = computeFront();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));
    
    float velocity = m_moveSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        velocity *= 2.5f; // Sprint
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) m_position += up * velocity;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) m_position -= up * velocity;
    
    // Zoom with +/- keys
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        m_fov -= 50.0f * deltaTime;
        if (m_fov < 20.0f) m_fov = 20.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        m_fov += 50.0f * deltaTime;
        if (m_fov > 90.0f) m_fov = 90.0f;
    }
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 front = computeFront();
    return glm::lookAt(m_position, m_position + front, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::setControlsEnabled(bool enabled) {
    m_controlsEnabled = enabled;
    if (s_instance) {
        // This would need access to the GLFW window to set cursor mode
        // For now, we'll handle this in the main application
    }
}

glm::vec3 Camera::computeFront() const {
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    
    glm::vec3 front;
    front.x = cosf(yawRad) * cosf(pitchRad);
    front.y = sinf(pitchRad);
    front.z = sinf(yawRad) * cosf(pitchRad);
    
    return glm::normalize(front);
}

void Camera::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!s_instance || !s_instance->m_controlsEnabled) return;
    
    if (s_instance->m_firstMouse) {
        s_instance->m_lastMouseX = xpos;
        s_instance->m_lastMouseY = ypos;
        s_instance->m_firstMouse = false;
    }
    
    double xoffset = xpos - s_instance->m_lastMouseX;
    double yoffset = s_instance->m_lastMouseY - ypos; // Reversed Y
    
    s_instance->m_lastMouseX = xpos;
    s_instance->m_lastMouseY = ypos;
    
    s_instance->m_yaw += static_cast<float>(xoffset) * s_instance->m_mouseSensitivity;
    s_instance->m_pitch += static_cast<float>(yoffset) * s_instance->m_mouseSensitivity;
    
    if (s_instance->m_pitch > 89.0f) s_instance->m_pitch = 89.0f;
    if (s_instance->m_pitch < -89.0f) s_instance->m_pitch = -89.0f;
}

void Camera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!s_instance || !s_instance->m_controlsEnabled) return;
    
    s_instance->m_fov -= static_cast<float>(yoffset) * 2.0f;
    if (s_instance->m_fov < 20.0f) s_instance->m_fov = 20.0f;
    if (s_instance->m_fov > 90.0f) s_instance->m_fov = 90.0f;
}

void Camera::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!s_instance) return;
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_B) {
            s_instance->m_controlsEnabled = !s_instance->m_controlsEnabled;
            if (s_instance->m_controlsEnabled) {
                s_instance->m_firstMouse = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        else if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        else if (key == GLFW_KEY_F) {
            // Toggle FPS display - we need to access the BaseScene's FPSRenderer
            // This will be handled by the BaseScene's key handling
            s_instance->m_fpsToggleRequested = true;
        }
    }
}
