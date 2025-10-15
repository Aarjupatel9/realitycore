#pragma once

#include <glm/glm.hpp>

/**
 * RenderConfig - Rendering system configuration
 * 
 * Controls background color, lighting, and rendering parameters.
 * All values have sensible defaults for typical 3D scenes.
 */
struct RenderConfig {
    // Background
    glm::vec3 backgroundColor = glm::vec3(0.5f, 0.8f, 1.0f);  // Sky blue default
    
    // Lighting
    glm::vec3 lightPosition = glm::vec3(10.0f, 10.0f, 10.0f);  // Top-right light
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);        // White light
    float ambientIntensity = 0.5f;                              // 50% ambient (0.0 = dark, 1.0 = bright)
    
    // Grid (if used)
    glm::vec3 gridColor = glm::vec3(0.3f, 0.3f, 0.3f);         // Gray
    float gridLineWidth = 2.0f;                                 // Line thickness in pixels
    float gridElevation = 0.01f;                                // Elevation above ground (prevents z-fighting)
    
    // Shader
    const char* shaderVersion = "#version 330 core";            // OpenGL 3.3 (change if needed)
};

