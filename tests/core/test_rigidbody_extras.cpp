#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "core/RigidBody3D.h"
#include "shapes/Box.h"

TEST(RigidBody3D, StaticBodyNoIntegration)
{
    auto shape = std::make_unique<Box>(1,1,1);
    RigidBody3D body(std::move(shape), 1.0f);
    body.setStatic(true);
    body.addForce(glm::vec3(100.0f, 0.0f, 0.0f));
    auto pos_before = body.getPosition();
    body.integrate(1.0f);
    EXPECT_EQ(body.getPosition().x, pos_before.x);
    EXPECT_EQ(body.getLinearVelocity().x, 0.0f);
    EXPECT_EQ(body.getInverseMass(), 0.0f);
}

TEST(RigidBody3D, DensityUpdatesMassAndInertia)
{
    auto shape = std::make_unique<Box>(2,3,4);
    RigidBody3D body(std::move(shape), 1.0f);

    float density = 1000.0f;
    body.setDensity(density);

    // Mass should be volume * density
    float volume = (2.0f*3.0f*4.0f);
    EXPECT_NEAR(body.getMass(), volume * density, 1e-3f);

    // Inertia should be recomputed (spot check: diagonal entries > 0)
    auto I = body.m_inertiaTensor;
    EXPECT_GT(I[0][0], 0.0f);
    EXPECT_GT(I[1][1], 0.0f);
    EXPECT_GT(I[2][2], 0.0f);
}

TEST(RigidBody3D, ScaleUpdatesInertiaAndAABB)
{
    auto shape = std::make_unique<Box>(1,1,1);
    RigidBody3D body(std::move(shape), 2.0f);
    body.setScale(glm::vec3(2.0f, 1.0f, 0.5f));

    // Inertia changes with scale; we just ensure non-zero and stable
    auto I = body.m_inertiaTensor;
    EXPECT_GT(I[0][0], 0.0f);

    // AABB via shape access
    auto* s = body.getShape();
    auto min = s->getBoundingBoxMin();
    auto max = s->getBoundingBoxMax();
    EXPECT_NEAR(min.x, -1.0f, 1e-4f);
    EXPECT_NEAR(max.x, 1.0f, 1e-4f);
}

TEST(RigidBody3D, AddForceAtPointGeneratesTorque)
{
    auto shape = std::make_unique<Box>(1,1,1);
    RigidBody3D body(std::move(shape), 1.0f);

    body.setPosition(glm::vec3(0.0f));
    glm::vec3 point(0.0f, 1.0f, 0.0f);
    glm::vec3 force(1.0f, 0.0f, 0.0f);

    body.addForceAtPoint(force, point);
    EXPECT_NE(body.getTorque(), glm::vec3(0.0f));
}

TEST(RigidBody3D, QuaternionRemainsNormalized)
{
    auto shape = std::make_unique<Box>(1,1,1);
    RigidBody3D body(std::move(shape), 1.0f);

    body.addTorque(glm::vec3(0.0f, 1.0f, 0.0f));
    body.integrate(0.016f);

    float len = glm::length(body.getRotation());
    EXPECT_NEAR(len, 1.0f, 1e-4f);
}
