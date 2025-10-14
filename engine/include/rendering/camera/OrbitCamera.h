#pragma once

#include "../../../src/rendering/Camera.h"
#include "../../bullet/BulletRigidBody.h"
#include <glm/glm.hpp>

/**
 * @brief A camera that orbits around a target object
 * 
 * OrbitCamera continuously rotates around a target object at a fixed
 * radius and height, providing a dynamic circular view of the scene.
 */
class OrbitCamera : public Camera {
private:
    BulletRigidBody* m_centerObject;
    float m_radius;
    float m_height;
    float m_orbitSpeed;
    float m_currentAngle = 0.0f;
    
public:
    /**
     * @brief Construct an orbit camera
     * @param center Target object to orbit around (can be null for world origin)
     * @param radius Distance from the center point
     * @param height Height above the center point
     * @param speed Rotation speed in radians per second
     */
    OrbitCamera(BulletRigidBody* center, float radius, float height, float speed);
    
    /**
     * @brief Update camera position along orbit path
     * @param window GLFW window (ignored for orbit camera)
     * @param deltaTime Time elapsed since last frame
     */
    void update(GLFWwindow* window, float deltaTime);
    
    /**
     * @brief Set the orbit radius
     * @param radius New orbit radius
     */
    void setRadius(float radius) { m_radius = radius; }
    
    /**
     * @brief Set the orbit height
     * @param height New orbit height
     */
    void setHeight(float height) { m_height = height; }
    
    /**
     * @brief Set the orbit speed
     * @param speed New orbit speed in radians per second
     */
    void setOrbitSpeed(float speed) { m_orbitSpeed = speed; }
    
    /**
     * @brief Set the current angle
     * @param angle New angle in radians
     */
    void setCurrentAngle(float angle) { m_currentAngle = angle; }
    
    /**
     * @brief Get the orbit radius
     * @return Current orbit radius
     */
    float getRadius() const { return m_radius; }
    
    /**
     * @brief Get the orbit height
     * @return Current orbit height
     */
    float getHeight() const { return m_height; }
    
    /**
     * @brief Get the orbit speed
     * @return Current orbit speed
     */
    float getOrbitSpeed() const { return m_orbitSpeed; }
    
    /**
     * @brief Get the current angle
     * @return Current angle in radians
     */
    float getCurrentAngle() const { return m_currentAngle; }
    
    /**
     * @brief Get the center object
     * @return Pointer to center object (can be null)
     */
    BulletRigidBody* getCenterObject() const { return m_centerObject; }

private:
    /**
     * @brief Calculate yaw and pitch to look at center
     * @param position Camera position
     * @param center Center position
     */
    void calculateLookAtCenter(glm::vec3 position, glm::vec3 center);
};
