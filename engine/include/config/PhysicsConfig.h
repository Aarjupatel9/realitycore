#pragma once

#include <glm/glm.hpp>

/**
 * PhysicsConfig - Physics simulation configuration
 * 
 * Controls Bullet Physics solver parameters, simulation timing, and quality.
 * Defaults provide a good balance of accuracy and performance.
 */
struct PhysicsConfig {
    // Gravity
    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);         // Earth gravity (can set to 0 for space)
    
    // Bullet solver parameters (quality vs performance)
    int solverIterations = 50;                                  // Collision resolution quality (10=fast, 100=accurate)
    float erp = 0.2f;                                           // Error reduction parameter (0.0-1.0, higher=stiffer)
    float erp2 = 0.2f;                                          // ERP for contact constraints
    float globalCfm = 0.0f;                                     // Constraint force mixing (softness)
    
    // Split impulse (prevents objects sinking into each other)
    bool enableSplitImpulse = true;                             // Recommended: true
    float splitImpulseThreshold = -0.002f;                      // When to apply split impulse
    
    // Continuous collision detection (for fast-moving objects)
    float ccdPenetration = 0.0001f;                             // Allowed CCD penetration (smaller=more precise)
    
    // Simulation timing
    float fixedTimeStep = 1.0f / 60.0f;                         // Physics tick rate (1/60 = 60 FPS)
    int maxSubSteps = 10;                                       // Max substeps per frame (prevents spiral of death)
    
    // Debug
    bool enableDebugDraw = false;                               // Render collision shapes
    int debugPrintInterval = 0;                                 // Frames between debug prints (0 = off)
};

