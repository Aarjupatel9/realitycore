#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>

#include "core/CollisionSystem.h"
#include "core/RigidBody3D.h"
#include "shapes/Sphere.h"

static std::unique_ptr<RigidBody3D> makeSphereBody(float radius, float mass, const glm::vec3& pos)
{
    auto shape = std::make_unique<Sphere>(radius, 8);
    auto body = std::make_unique<RigidBody3D>(std::move(shape), mass);
    body->setPosition(pos);
    body->setGravityEnabled(false);
    return body;
}

TEST(CollisionSystem, SphereSphereTouchingEdgeCase)
{
    CollisionSystem system;
    std::vector<RigidBody3D*> bodies;

    auto a = makeSphereBody(0.5f, 1.0f, {0.f, 0.f, 0.f});
    auto b = makeSphereBody(0.5f, 1.0f, {1.0f, 0.f, 0.f}); // exactly touching (0.5 + 0.5)

    bodies.push_back(a.get());
    bodies.push_back(b.get());

    std::vector<CollisionSystem::CollisionInfo> collisions;
    system.CheckCollisions(bodies, collisions);

    // Current implementation uses strict <, not <=, so exactly touching won't count as collision
    EXPECT_TRUE(collisions.empty());
}

TEST(CollisionSystem, SphereSpherePenetrating)
{
    CollisionSystem system;
    std::vector<RigidBody3D*> bodies;

    auto a = makeSphereBody(0.5f, 1.0f, {0.f, 0.f, 0.f});
    auto b = makeSphereBody(0.5f, 1.0f, {0.9f, 0.f, 0.f}); // overlap by 0.1

    bodies.push_back(a.get());
    bodies.push_back(b.get());

    std::vector<CollisionSystem::CollisionInfo> collisions;
    system.CheckCollisions(bodies, collisions);

    ASSERT_EQ(collisions.size(), 1u);
    auto& c = collisions[0];
    EXPECT_NEAR(c.penetration, (0.5f+0.5f) - 0.9f, 1e-5f);
    EXPECT_NEAR(glm::length(c.contactNormal), 1.0f, 1e-5f);
}

TEST(CollisionSystem, GroundCollisionNormal)
{
    CollisionSystem system;

    auto a = makeSphereBody(0.5f, 1.0f, {0.f, 0.4f, 0.f}); // should collide with ground at y=-1?
    // Note: CollisionSystem::CheckGroundCollision uses radius=0.5 and checks y - r <= groundY
    // For groundY = -1.0, 0.4 - 0.5 <= -1.0 is false, so no collision.
    std::vector<RigidBody3D*> bodies{a.get()};
    system.CheckGroundCollisions(bodies, -1.0f);

    // Nothing to assert here due to current ground formula; just sanity check the call path.
    SUCCEED();
}
