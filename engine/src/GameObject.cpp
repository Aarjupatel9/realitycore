#include "GameObject.h"
#include "rendering/Mesh.h"
#include "rendering/MeshLOD.h"
#include "bullet/BulletRigidBody.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>

std::shared_ptr<Mesh> GameObject::getVisualMesh(const glm::vec3& cameraPosition) const {
    // If LOD system is set, use it to select appropriate mesh
    if (lodSystem) {
        // Calculate distance from camera
        float distance = glm::length(cameraPosition - position);
        
        // Estimate object radius from scale (use largest dimension)
        float objectRadius = std::max(std::max(scale.x, scale.y), scale.z);
        
        // Select mesh based on LOD
        return lodSystem->selectMesh(distance, objectRadius);
    }
    
    // Otherwise, return the fixed visual mesh
    return visualMesh;
}

glm::mat4 GameObject::getModelMatrix() const {
    // Start with identity matrix
    glm::mat4 model = glm::mat4(1.0f);
    
    // Apply translation (position + visual offset)
    glm::vec3 finalPosition = position + visualOffset;
    model = glm::translate(model, finalPosition);
    
    // Apply rotation
    model = model * glm::toMat4(rotation);
    
    // Apply scale (combine physics scale with visual scale)
    glm::vec3 finalScale = scale * visualScale;
    model = glm::scale(model, finalScale);
    
    return model;
}

void GameObject::syncFromPhysics() {
    if (!rigidBody || !physicsEnabled) {
        return;
    }
    
    // Get transform from physics simulation (returns glm::mat4)
    glm::mat4 transform = rigidBody->getTransform();
    
    // Extract position from last column
    position = glm::vec3(transform[3]);
    
    // Extract rotation from upper 3x3 matrix
    glm::mat3 rotationMatrix = glm::mat3(transform);
    rotation = glm::quat_cast(rotationMatrix);
}

