#pragma once

#include "../../engine/include/BaseScene.h"
#include "../../engine/src/rendering/Skybox.h"
#include "../../engine/src/rendering/Terrain.h"
#include "../../engine/src/rendering/GridRenderer.h"

/**
 * TerrainScene - Beautiful terrain with skybox and environment
 * 
 * Features:
 * - Procedural terrain generation
 * - Skybox rendering
 * - Grid overlay for navigation
 * - Environmental lighting
 */
class TerrainScene : public BaseScene {
public:
    TerrainScene();
    virtual ~TerrainScene() = default;
    
    // BaseScene interface
    bool initialize(GLFWwindow* window, const SceneConfig& config) override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;
    
    // Scene-specific methods
    void updateScene(float deltaTime) override;
    void renderScene() override;
    
    const char* getName() const override { return "Beautiful Terrain"; }
    const char* getDescription() const override { return "Procedural terrain with skybox, grass, and rocks"; }

private:
    // Rendering systems
    std::unique_ptr<Skybox> m_skybox;
    std::unique_ptr<Terrain> m_terrain;
    std::unique_ptr<GridRenderer> m_gridRenderer;
    
    // Lighting
    glm::vec3 m_sunDirection;
    
    // Terrain parameters - simple flat test
    static constexpr int TERRAIN_SIZE = 32;  // Very small for testing
    static constexpr float TERRAIN_SCALE = 1.0f;  // 32 * 1.0 = 32 units total (bigger)
    static constexpr float TERRAIN_HEIGHT = 0.0f;  // Flat terrain
    static constexpr float TERRAIN_ROUGHNESS = 0.0f;  // No variation
    
    // Scene initialization
    void initializeCamera();
    void initializeObjects() override;
    void initializeTerrain();
    void initializeSkybox();
    void initializeGrid();
};
