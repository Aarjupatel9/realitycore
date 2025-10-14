#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>

#include "core/World.h"
#include "core/RigidBody3D.h"
#include "shapes/Sphere.h"
#include "core/PhysicsConstants.h"

TEST(World, DeterministicMultiStepNoForces)
{
    World world(glm::vec3(0.0f)); // no gravity

    auto shape = std::make_unique<Sphere>(0.5f, 8);
    RigidBody3D body(std::move(shape), 1.0f);

    body.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    body.setLinearVelocity(glm::vec3(1.0f, 0.0f, 0.0f));
    body.setGravityEnabled(false);

    world.AddBody(&body);

    float dt = 0.1f;
    // Run 10 steps
    for (int i = 0; i < 10; ++i) {
        world.Update(dt);
    }

    // Expected: position advances roughly by sum of damped v*dt. Since damping applies each step,
    // final position < 1.0. We check determinism by recomputing the same sequence.

    // Recompute expected with same damping application as integrate(): v *= damping; v += a*dt (a=0)
    float damping = body.m_linearDamping;
    float v = 1.0f;
    float x = 0.0f;
    for (int i = 0; i < 10; ++i) {
        v *= damping;
        x += v * dt;
    }

    EXPECT_NEAR(body.getPosition().x, x, 1e-4f);
}
