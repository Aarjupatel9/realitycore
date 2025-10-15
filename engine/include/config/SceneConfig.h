#pragma once

#include "RenderConfig.h"
#include "CameraConfig.h"
#include "PhysicsConfig.h"

/**
 * SceneConfig - Main configuration aggregator
 * 
 * Combines all subsystem configurations into one convenient struct.
 * Pass to BaseScene::initialize() to configure the entire scene.
 * 
 * Usage:
 *   SceneConfig config;
 *   config.windowWidth = 1920;
 *   config.windowHeight = 1080;
 *   config.rendering.backgroundColor = glm::vec3(0.1f, 0.1f, 0.15f);
 *   config.camera.fov = 60.0f;
 *   scene->initialize(window, config);
 */
struct SceneConfig {
    // Subsystem configurations
    RenderConfig rendering;
    CameraConfig camera;
    PhysicsConfig physics;
    
    // Window dimensions (must be set from actual window)
    unsigned int windowWidth = 800;      // Default fallback
    unsigned int windowHeight = 600;     // Default fallback
    
    // Helper to get aspect ratio
    float getAspectRatio() const {
        if (windowHeight == 0) return 800.0f / 600.0f;  // Safety fallback
        return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    }
};

