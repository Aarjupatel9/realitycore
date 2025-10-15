#pragma once

#include "../../engine/include/BaseScene.h"

/**
 * BasicDemoScene - Simple physics demo with cube and sphere
 * 
 * Features:
 * - Cube and sphere with gravity
 * - Ground collision detection
 * - Basic physics simulation
 * - Performance testing mode (optional)
 */
class BasicDemoScene : public BaseScene {
public:
    BasicDemoScene();
    virtual ~BasicDemoScene() = default;
    
    // BaseScene interface
    bool initialize(GLFWwindow* window, const SceneConfig& config) override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;
    
    // Scene-specific methods
    void updateScene(float deltaTime) override;
    void renderScene() override;
    
    const char* getName() const override { return "Basic Demo"; }
    const char* getDescription() const override { return "Cube and sphere with gravity and ground collision"; }
    
    // Performance testing
    void setPerformanceTestMode(bool enabled) { m_performanceTestMode = enabled; }

private:
    // Performance testing mode
    bool m_performanceTestMode = false;
    
    // Scene initialization
    void initializeCamera();
    void initializeObjects() override;
    void createPerformanceTestObjects();
};
