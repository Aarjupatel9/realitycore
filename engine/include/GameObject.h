#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Forward declarations
class Mesh;
class MeshLOD;
class BulletRigidBody;

/**
 * @brief Generic game object that decouples visual representation from physics collision
 * 
 * This is a Unity/Unreal-like GameObject that separates:
 * - Visual mesh (can be high-poly custom mesh)
 * - Collision shape (optimized for physics)
 * 
 * Usage:
 *   GameObject car;
 *   car.visualMesh = MeshLoader::load("car.obj");           // 50k vertices
 *   car.rigidBody->setCollisionShape(boxShape);             // Simple box
 *   car.color = glm::vec3(1.0f, 0.0f, 0.0f);               // Red
 */
struct GameObject {
    // === VISUAL PROPERTIES ===
    
    /**
     * @brief Visual mesh for rendering (can be high-poly)
     * - Can be custom mesh loaded from file (.obj, .fbx, etc.)
     * - Can be procedurally generated mesh
     * - Completely independent from collision shape
     */
    std::shared_ptr<Mesh> visualMesh = nullptr;
    
    /**
     * @brief LOD system for visual mesh (optional)
     * - Automatically selects mesh quality based on distance
     * - If set, overrides visualMesh
     */
    std::shared_ptr<MeshLOD> lodSystem = nullptr;
    
    /**
     * @brief Object color (used in shader)
     */
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    
    /**
     * @brief Visual scale (independent of physics scale)
     * - Allows visual mesh to be scaled differently from collision
     */
    glm::vec3 visualScale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    /**
     * @brief Visual offset from physics position
     * - Useful when visual mesh origin doesn't match collision center
     */
    glm::vec3 visualOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    
    // === PHYSICS PROPERTIES ===
    
    /**
     * @brief Physics rigid body (contains collision shape)
     * - Collision shape can be primitive (box, sphere, capsule)
     * - Can be compound shape (multiple primitives)
     * - Can be convex hull (low-poly approximation)
     */
    std::unique_ptr<BulletRigidBody> rigidBody = nullptr;
    
    // === TRANSFORM ===
    
    /**
     * @brief World position (from physics simulation)
     */
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    /**
     * @brief World rotation (from physics simulation)
     */
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    
    /**
     * @brief World scale (from physics simulation)
     */
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    // === METADATA ===
    
    /**
     * @brief Object name (for debugging)
     */
    std::string name = "GameObject";
    
    /**
     * @brief User-defined tag (for grouping, filtering)
     */
    std::string tag = "";
    
    /**
     * @brief Enable/disable rendering
     */
    bool visible = true;
    
    /**
     * @brief Enable/disable physics simulation
     */
    bool physicsEnabled = true;
    
    // === HELPER METHODS ===
    
    /**
     * @brief Get the final visual mesh to render
     * - Returns LOD mesh if LOD system is set, otherwise returns visualMesh
     * @param cameraPosition Position of the camera for LOD calculation
     * @return Mesh to render, or nullptr if none available
     */
    std::shared_ptr<Mesh> getVisualMesh(const glm::vec3& cameraPosition) const;
    
    /**
     * @brief Get the model matrix for rendering
     * @return 4x4 transformation matrix combining position, rotation, scale, and visual offset
     */
    glm::mat4 getModelMatrix() const;
    
    /**
     * @brief Update position and rotation from physics simulation
     * - Should be called each frame to sync visual with physics
     */
    void syncFromPhysics();
};

