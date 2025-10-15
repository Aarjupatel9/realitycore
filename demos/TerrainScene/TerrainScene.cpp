#include "TerrainScene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

TerrainScene::TerrainScene() {
    // Set sun direction (afternoon sun)
    m_sunDirection = glm::normalize(glm::vec3(-0.5f, -0.8f, -0.3f));
}

bool TerrainScene::initialize(GLFWwindow* window, const SceneConfig& config) {
    // Call parent initialize to setup common components
    if (!BaseScene::initialize(window, config)) {
        return false;
    }
    
    // Initialize scene-specific components
    initializeCamera();
    
    std::cout << "Terrain Scene initialized successfully!" << std::endl;
    std::cout << "Terrain size: " << TERRAIN_SIZE << "x" << TERRAIN_SIZE << std::endl;
    std::cout << "Use WASD to move, mouse to look around" << std::endl;
    
    return true;
}

void TerrainScene::initializeCamera() {
    // Initialize camera closer to see the terrain
    m_camera->setPosition(glm::vec3(5.0f, 5.0f, 5.0f)); // Closer position
    m_camera->setYaw(-45.0f);   // Looking towards center
    m_camera->setPitch(-30.0f); // Looking down at angle
}

void TerrainScene::initializeObjects() {
    // Initialize terrain-specific objects
    initializeTerrain();
    initializeSkybox();
    initializeGrid();
}

void TerrainScene::initializeTerrain() {
    // Initialize terrain
    m_terrain = std::make_unique<Terrain>();
    if (!m_terrain->initialize(TERRAIN_SIZE, TERRAIN_SIZE, TERRAIN_SCALE, TERRAIN_HEIGHT, TERRAIN_ROUGHNESS)) {
        std::cerr << "Failed to initialize terrain" << std::endl;
        throw std::runtime_error("Terrain initialization failed");
    }
}

void TerrainScene::initializeSkybox() {
    // Initialize skybox
    m_skybox = std::make_unique<Skybox>();
    if (!m_skybox->initialize()) {
        std::cerr << "Failed to initialize skybox" << std::endl;
        throw std::runtime_error("Skybox initialization failed");
    }
}

void TerrainScene::initializeGrid() {
    // Initialize grid renderer for debugging
    m_gridRenderer = std::make_unique<GridRenderer>();
    if (!m_gridRenderer->initialize(32.0f, 16)) { // 16x16 grid over 32x32 units
        std::cerr << "Failed to initialize grid renderer" << std::endl;
        throw std::runtime_error("Grid renderer initialization failed");
    }
    m_gridRenderer->setColor(glm::vec3(0.1f, 0.1f, 0.1f)); // Very dark lines for better visibility
}

void TerrainScene::update(float deltaTime) {
    // BaseScene handles common updates
    BaseScene::update(deltaTime);
    updateScene(deltaTime);
}

void TerrainScene::render() {
    // BaseScene handles common rendering
    BaseScene::render();
    renderScene();
}

void TerrainScene::updateScene(float deltaTime) {
    // Terrain and skybox don't need updates in this simple implementation
}

void TerrainScene::renderScene() {
    // Set lighting uniforms for terrain rendering
    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 projection = m_camera->getProjectionMatrix(800.0f / 600.0f); // Default aspect ratio
    
    // Render skybox first (background)
    if (m_skybox) {
        m_skybox->render(view, projection);
    }
    
    // Render terrain with model matrix
    if (m_terrain) {
        glm::mat4 model = glm::mat4(1.0f); // Identity matrix
        m_terrain->render(model, view, projection, m_sunDirection);
    }
    
    // Render grid overlay
    if (m_gridRenderer) {
        m_gridRenderer->render(view, projection);
    }
}

void TerrainScene::cleanup() {
    std::cout << "Cleaning up Terrain Scene..." << std::endl;
    
    // Cleanup terrain-specific resources
    m_terrain.reset();
    m_skybox.reset();
    m_gridRenderer.reset();
    
    // BaseScene handles common cleanup
}
