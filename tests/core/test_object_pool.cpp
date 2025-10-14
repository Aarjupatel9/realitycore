#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>

#include "core/PhysicsObjectPool.h"
#include "core/RigidBody3D.h"
#include "shapes/Box.h"
#include "shapes/Sphere.h"

TEST(PhysicsObjectPool, AcquireReleaseReuse)
{
    auto& pool = PhysicsObjectPool::getInstance();
    pool.clear();

    size_t created_before = pool.getTotalCreated();
    size_t reused_before = pool.getTotalReused();
    size_t avail_before = pool.getTotalAvailable();

    {
        auto body1 = pool.acquireRigidBody(std::make_unique<Box>(1,2,3), 2.0f);
        ASSERT_NE(body1, nullptr);
        EXPECT_EQ(pool.getTotalCreated(), created_before + 1);
        EXPECT_EQ(pool.getTotalReused(), reused_before);

        auto body2 = pool.acquireRigidBody(std::make_unique<Box>(1,2,3), 2.0f);
        ASSERT_NE(body2, nullptr);
        EXPECT_EQ(pool.getTotalCreated(), created_before + 2);

        // Release both back to pool
        pool.releaseRigidBody(std::move(body1));
        pool.releaseRigidBody(std::move(body2));
        EXPECT_GE(pool.getTotalAvailable(), avail_before + 2);
    }

    // Acquire again with the same shape+mass, should reuse
    {
        auto body3 = pool.acquireRigidBody(std::make_unique<Box>(1,2,3), 2.0f);
        ASSERT_NE(body3, nullptr);
        EXPECT_EQ(pool.getTotalReused(), reused_before + 1);
    }
}
