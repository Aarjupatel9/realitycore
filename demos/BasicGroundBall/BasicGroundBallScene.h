#pragma once

#include "BaseScene.h"

class BasicGroundBallScene : public BaseScene {
public:
    BasicGroundBallScene();
    virtual ~BasicGroundBallScene() = default;

    // BaseScene overrides
    bool initialize(GLFWwindow* window, const SceneConfig& config) override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;
    void initializeObjects() override;
    
    const char* getName() const override { return "BasicGroundBall Scene"; }
    const char* getDescription() const override { return "Ground plane with ball placed on top - demonstrates static/dynamic object interaction"; }
};
