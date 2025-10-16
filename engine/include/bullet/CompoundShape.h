#pragma once

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>

/**
 * @brief Compound collision shape builder
 * 
 * Allows combining multiple primitive shapes into a single collision shape.
 * Useful for complex objects like cars (body + wheels), characters, etc.
 * 
 * Usage:
 *   CompoundShape carShape;
 *   carShape.addBox(glm::vec3(0,0,0), glm::vec3(4,1,2));        // Body
 *   carShape.addSphere(glm::vec3(-1.5,-0.5,1), 0.4f);          // Wheel 1
 *   carShape.addSphere(glm::vec3(1.5,-0.5,1), 0.4f);           // Wheel 2
 *   carShape.addSphere(glm::vec3(-1.5,-0.5,-1), 0.4f);         // Wheel 3
 *   carShape.addSphere(glm::vec3(1.5,-0.5,-1), 0.4f);          // Wheel 4
 *   btCompoundShape* shape = carShape.build();
 */
class CompoundShape {
public:
    CompoundShape();
    ~CompoundShape();
    
    /**
     * @brief Add a box shape to the compound
     * @param position Local position offset
     * @param size Box dimensions (half-extents will be size/2)
     * @param rotation Local rotation (optional)
     */
    void addBox(const glm::vec3& position, const glm::vec3& size, const glm::quat& rotation = glm::quat(1,0,0,0));
    
    /**
     * @brief Add a sphere shape to the compound
     * @param position Local position offset
     * @param radius Sphere radius
     */
    void addSphere(const glm::vec3& position, float radius);
    
    /**
     * @brief Add a cylinder shape to the compound
     * @param position Local position offset
     * @param radius Cylinder radius
     * @param height Cylinder height
     * @param rotation Local rotation (optional)
     */
    void addCylinder(const glm::vec3& position, float radius, float height, const glm::quat& rotation = glm::quat(1,0,0,0));
    
    /**
     * @brief Add a capsule shape to the compound
     * @param position Local position offset
     * @param radius Capsule radius
     * @param height Capsule height (total height including caps)
     * @param rotation Local rotation (optional)
     */
    void addCapsule(const glm::vec3& position, float radius, float height, const glm::quat& rotation = glm::quat(1,0,0,0));
    
    /**
     * @brief Add a cone shape to the compound
     * @param position Local position offset
     * @param radius Cone base radius
     * @param height Cone height
     * @param rotation Local rotation (optional)
     */
    void addCone(const glm::vec3& position, float radius, float height, const glm::quat& rotation = glm::quat(1,0,0,0));
    
    /**
     * @brief Add a custom shape to the compound
     * @param position Local position offset
     * @param shape Custom Bullet collision shape (takes ownership)
     * @param rotation Local rotation (optional)
     */
    void addCustomShape(const glm::vec3& position, btCollisionShape* shape, const glm::quat& rotation = glm::quat(1,0,0,0));
    
    /**
     * @brief Build the final compound shape
     * @return Pointer to btCompoundShape (caller takes ownership)
     * @note After calling build(), this object is invalidated (shapes are moved)
     */
    btCompoundShape* build();
    
    /**
     * @brief Get the number of child shapes
     * @return Number of shapes in the compound
     */
    int getShapeCount() const;
    
    /**
     * @brief Clear all shapes
     */
    void clear();
    
    /**
     * @brief Create a car-like compound shape (body + wheels)
     * @param bodySize Body dimensions (half-extents)
     * @param wheelRadius Wheel radius
     * @param wheelWidth Wheel width
     * @param wheelOffset Distance from center to wheel
     * @return Built compound shape
     */
    static btCompoundShape* createCarShape(const glm::vec3& bodySize, float wheelRadius, float wheelWidth, float wheelOffset);
    
    /**
     * @brief Create a character-like compound shape (capsule + head)
     * @param height Total character height
     * @param radius Character radius
     * @return Built compound shape
     */
    static btCompoundShape* createCharacterShape(float height, float radius);

private:
    btCompoundShape* m_compoundShape;
    std::vector<btCollisionShape*> m_childShapes; // Track for cleanup
    bool m_built;
    
    btTransform createTransform(const glm::vec3& position, const glm::quat& rotation) const;
};

