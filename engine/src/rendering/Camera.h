#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

// Free-fly camera with mouse look and keyboard movement
class Camera {
public:
    Camera();
    virtual ~Camera() = default;
    
    // Update camera based on input
    void update(GLFWwindow* window, float deltaTime);
    
    // Get view matrix
    glm::mat4 getViewMatrix() const;
    
    // Get projection matrix
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    // Camera properties
    glm::vec3 getPosition() const { return m_position; }
    void setPosition(const glm::vec3& position) { m_position = position; }
    
    float getFOV() const { return m_fov; }
    void setFOV(float fov) { m_fov = fov; }
    
    float getYaw() const { return m_yaw; }
    void setYaw(float yaw) { m_yaw = yaw; }
    
    float getPitch() const { return m_pitch; }
    void setPitch(float pitch) { m_pitch = pitch; }
    
    glm::vec3 getFront() const { return computeFront(); }
    
    bool isControlsEnabled() const { return m_controlsEnabled; }
    void setControlsEnabled(bool enabled);
    
    // Check if FPS toggle was requested
    bool checkFpsToggleRequest() {
        if (m_fpsToggleRequested) {
            m_fpsToggleRequested = false;
            return true;
        }
        return false;
    }
    
    // Static callbacks
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    
    float m_yaw;
    float m_pitch;
    float m_fov;
    
    float m_moveSpeed;
    float m_mouseSensitivity;
    
    bool m_controlsEnabled;
    bool m_firstMouse;
    bool m_fpsToggleRequested;
    double m_lastMouseX;
    double m_lastMouseY;
    
    glm::vec3 computeFront() const;
    
    // Static instance for callbacks
    static Camera* s_instance;
};