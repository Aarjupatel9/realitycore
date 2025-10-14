#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>

#include "core/World.h"
#include "core/RigidBody3D.h"
#include "shapes/Sphere.h"
#include "core/PhysicsConstants.h"

TEST(World, AppliesGravityAndIntegrates)
{
    World world(Physics::DEFAULT_GRAVITY);

    auto shape = std::make_unique<Sphere>(0.5f, 8);
    RigidBody3D body(std::move(shape), 2.0f);

    body.setPosition(glm::vec3(0.0f, 10.0f, 0.0f));
    body.setLinearVelocity(glm::vec3(0.0f));

    world.AddBody(&body);

    float dt = 0.5f; // seconds
    world.Update(dt);

    // Expected: gravity acceleration = -9.81 m/s^2 in Y
    // integrate(): v is damped first (v=0 => no effect), then a*dt is added
    // So final v_y = -9.81 * dt, and y += v*dt
    float expected_vy = -Physics::GRAVITY * dt;
    EXPECT_NEAR(body.getLinearVelocity().y, expected_vy, 1e-4f);

    float expected_y = 10.0f + expected_vy * dt;
    EXPECT_NEAR(body.getPosition().y, expected_y, 1e-3f);
}
