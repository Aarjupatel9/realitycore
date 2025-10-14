#pragma once

#include "BaseScene.h"

// Forward declaration
class BulletRigidBody;

class BallCollision2Scene : public BaseScene {
public:
    BallCollision2Scene();
    virtual ~BallCollision2Scene() = default;

    // BaseScene overrides
    bool initialize(GLFWwindow* window) override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;
    void initializeObjects() override;
    
    const char* getName() const override { return "BallCollision2 Scene"; }
    const char* getDescription() const override { return "Generated BallCollision2 scene"; }

private:
    // Object references for camera controller
    BulletRigidBody* m_ball1;
    BulletRigidBody* m_ball2;
    BulletRigidBody* m_ground;
};
