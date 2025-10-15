#pragma once

#include "BaseScene.h"

class BallFreeFallScene : public BaseScene {
public:
    BallFreeFallScene();
    virtual ~BallFreeFallScene() = default;

    // BaseScene overrides
    bool initialize(GLFWwindow* window, const SceneConfig& config) override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;
    void initializeObjects() override;
    
    const char* getName() const override { return "BallFreeFall Scene"; }
    const char* getDescription() const override { return "Simple ball falling under gravity - demonstrates basic Bullet Physics"; }
};
