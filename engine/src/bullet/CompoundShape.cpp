#include "bullet/CompoundShape.h"
#include <iostream>

CompoundShape::CompoundShape() : m_built(false) {
    m_compoundShape = new btCompoundShape();
}

CompoundShape::~CompoundShape() {
    if (!m_built && m_compoundShape) {
        // If build() wasn't called, we still own the shapes
        for (auto* shape : m_childShapes) {
            delete shape;
        }
        delete m_compoundShape;
    }
    // If build() was called, ownership was transferred to caller
}

void CompoundShape::addBox(const glm::vec3& position, const glm::vec3& size, const glm::quat& rotation) {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: Cannot add shapes after build()" << std::endl;
        return;
    }
    
    btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
    btBoxShape* boxShape = new btBoxShape(halfExtents);
    
    btTransform transform = createTransform(position, rotation);
    m_compoundShape->addChildShape(transform, boxShape);
    m_childShapes.push_back(boxShape);
}

void CompoundShape::addSphere(const glm::vec3& position, float radius) {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: Cannot add shapes after build()" << std::endl;
        return;
    }
    
    btSphereShape* sphereShape = new btSphereShape(radius);
    
    btTransform transform = createTransform(position, glm::quat(1,0,0,0));
    m_compoundShape->addChildShape(transform, sphereShape);
    m_childShapes.push_back(sphereShape);
}

void CompoundShape::addCylinder(const glm::vec3& position, float radius, float height, const glm::quat& rotation) {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: Cannot add shapes after build()" << std::endl;
        return;
    }
    
    btVector3 halfExtents(radius, height * 0.5f, radius);
    btCylinderShape* cylinderShape = new btCylinderShape(halfExtents);
    
    btTransform transform = createTransform(position, rotation);
    m_compoundShape->addChildShape(transform, cylinderShape);
    m_childShapes.push_back(cylinderShape);
}

void CompoundShape::addCapsule(const glm::vec3& position, float radius, float height, const glm::quat& rotation) {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: Cannot add shapes after build()" << std::endl;
        return;
    }
    
    // Capsule height is the cylindrical part only (caps add 2*radius)
    float cylinderHeight = height - (2.0f * radius);
    if (cylinderHeight < 0.0f) {
        std::cerr << "CompoundShape WARNING: Capsule height too small for radius, using minimum" << std::endl;
        cylinderHeight = 0.1f;
    }
    
    btCapsuleShape* capsuleShape = new btCapsuleShape(radius, cylinderHeight);
    
    btTransform transform = createTransform(position, rotation);
    m_compoundShape->addChildShape(transform, capsuleShape);
    m_childShapes.push_back(capsuleShape);
}

void CompoundShape::addCone(const glm::vec3& position, float radius, float height, const glm::quat& rotation) {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: Cannot add shapes after build()" << std::endl;
        return;
    }
    
    btConeShape* coneShape = new btConeShape(radius, height);
    
    btTransform transform = createTransform(position, rotation);
    m_compoundShape->addChildShape(transform, coneShape);
    m_childShapes.push_back(coneShape);
}

void CompoundShape::addCustomShape(const glm::vec3& position, btCollisionShape* shape, const glm::quat& rotation) {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: Cannot add shapes after build()" << std::endl;
        return;
    }
    
    if (!shape) {
        std::cerr << "CompoundShape ERROR: Cannot add null shape" << std::endl;
        return;
    }
    
    btTransform transform = createTransform(position, rotation);
    m_compoundShape->addChildShape(transform, shape);
    m_childShapes.push_back(shape);
}

btCompoundShape* CompoundShape::build() {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: build() already called" << std::endl;
        return nullptr;
    }
    
    m_built = true;
    btCompoundShape* result = m_compoundShape;
    m_compoundShape = nullptr; // Transfer ownership
    
    std::cout << "CompoundShape: Built compound shape with " << m_childShapes.size() << " child shapes" << std::endl;
    
    return result;
}

int CompoundShape::getShapeCount() const {
    return static_cast<int>(m_childShapes.size());
}

void CompoundShape::clear() {
    if (m_built) {
        std::cerr << "CompoundShape ERROR: Cannot clear after build()" << std::endl;
        return;
    }
    
    for (auto* shape : m_childShapes) {
        delete shape;
    }
    m_childShapes.clear();
    
    delete m_compoundShape;
    m_compoundShape = new btCompoundShape();
}

btCompoundShape* CompoundShape::createCarShape(const glm::vec3& bodySize, float wheelRadius, float wheelWidth, float wheelOffset) {
    CompoundShape builder;
    
    // Add car body (centered)
    builder.addBox(glm::vec3(0.0f, 0.0f, 0.0f), bodySize);
    
    // Calculate wheel positions
    float wheelY = -bodySize.y * 0.5f - wheelRadius * 0.5f; // Below body
    float wheelX = bodySize.x * 0.5f - wheelOffset;
    float wheelZ = bodySize.z * 0.5f - wheelOffset;
    
    // Add 4 wheels (cylinders rotated to align with X-axis)
    glm::quat wheelRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1));
    
    builder.addCylinder(glm::vec3(-wheelX, wheelY,  wheelZ), wheelRadius, wheelWidth, wheelRotation); // Front-left
    builder.addCylinder(glm::vec3( wheelX, wheelY,  wheelZ), wheelRadius, wheelWidth, wheelRotation); // Front-right
    builder.addCylinder(glm::vec3(-wheelX, wheelY, -wheelZ), wheelRadius, wheelWidth, wheelRotation); // Back-left
    builder.addCylinder(glm::vec3( wheelX, wheelY, -wheelZ), wheelRadius, wheelWidth, wheelRotation); // Back-right
    
    return builder.build();
}

btCompoundShape* CompoundShape::createCharacterShape(float height, float radius) {
    CompoundShape builder;
    
    // Body capsule
    float bodyHeight = height * 0.7f;
    builder.addCapsule(glm::vec3(0.0f, 0.0f, 0.0f), radius * 0.8f, bodyHeight);
    
    // Head sphere
    float headRadius = radius * 0.6f;
    float headY = bodyHeight * 0.5f + headRadius;
    builder.addSphere(glm::vec3(0.0f, headY, 0.0f), headRadius);
    
    return builder.build();
}

btTransform CompoundShape::createTransform(const glm::vec3& position, const glm::quat& rotation) const {
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    return transform;
}

