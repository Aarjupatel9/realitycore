#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <memory>

#include "shapes/Box.h"
#include "shapes/Sphere.h"

TEST(InertiaTensor, BoxAnalytical)
{
    Box box(2.0f, 3.0f, 4.0f); // w=2,h=3,d=4
    float mass = 5.0f;

    glm::mat3 I = box.getInertiaTensor(mass);
    float w = 2.0f, h = 3.0f, d = 4.0f;

    float Ixx = mass * (h*h + d*d) / 12.0f;
    float Iyy = mass * (w*w + d*d) / 12.0f;
    float Izz = mass * (w*w + h*h) / 12.0f;

    EXPECT_NEAR(I[0][0], Ixx, 1e-5f);
    EXPECT_NEAR(I[1][1], Iyy, 1e-5f);
    EXPECT_NEAR(I[2][2], Izz, 1e-5f);

    EXPECT_NEAR(I[0][1], 0.0f, 1e-6f);
    EXPECT_NEAR(I[0][2], 0.0f, 1e-6f);
    EXPECT_NEAR(I[1][0], 0.0f, 1e-6f);
    EXPECT_NEAR(I[1][2], 0.0f, 1e-6f);
    EXPECT_NEAR(I[2][0], 0.0f, 1e-6f);
    EXPECT_NEAR(I[2][1], 0.0f, 1e-6f);
}

TEST(InertiaTensor, SphereAnalytical)
{
    Sphere sphere(0.5f, 8);
    float mass = 2.0f;

    glm::mat3 I = sphere.getInertiaTensor(mass);
    float r = 0.5f;
    float expected = (2.0f/5.0f) * mass * r * r;

    EXPECT_NEAR(I[0][0], expected, 1e-5f);
    EXPECT_NEAR(I[1][1], expected, 1e-5f);
    EXPECT_NEAR(I[2][2], expected, 1e-5f);
}
