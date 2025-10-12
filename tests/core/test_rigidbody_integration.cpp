#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>

#include "core/RigidBody3D.h"
#include "shapes/Sphere.h"
#include "core/PhysicsConstants.h"

// Note: Including from engine/src for now due to public header gaps.
// We can migrate to engine/include once public API is consolidated.

TEST(RigidBody3D, IntegratesLinearMotionWithForce)
{
    // Arrange: sphere of mass 2 kg
    auto shape = std::make_unique<Sphere>(0.5f, 8);
    RigidBody3D body(std::move(shape), 2.0f);
    body.setPosition(glm::vec3(0.0f));
    body.setLinearVelocity(glm::vec3(0.0f));
    body.setGravityEnabled(false); // isolate applied force only

    // Apply a constant force F = (4, 0, 0) N for dt = 0.5 s
    glm::vec3 F(4.0f, 0.0f, 0.0f);
    float dt = 0.5f;

    body.addForce(F);
    body.integrate(dt);

    // Expected: a = F/m = 2 m/s^2, v = a*dt = 1 m/s, x = v*dt = 0.5 m
    EXPECT_NEAR(body.getLinearVelocity().x, 1.0f, 1e-5f);
    EXPECT_NEAR(body.getPosition().x, 0.5f, 1e-5f);
}

TEST(RigidBody3D, DampingAffectsVelocity)
{
    auto shape = std::make_unique<Sphere>(0.5f, 8);
    RigidBody3D body(std::move(shape), 1.0f);
    body.setLinearVelocity(glm::vec3(10.0f, 0.0f, 0.0f));
    body.setGravityEnabled(false);

    float dt = 1.0f;
    float damping = body.m_linearDamping; // default is 0.99
    body.integrate(dt);

    EXPECT_NEAR(body.getLinearVelocity().x, 10.0f * damping, 1e-5f);
}
