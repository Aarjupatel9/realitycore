#pragma once

#include "../../engine/include/BaseScene.h"
#include <random>

/**
 * Ball Collision Scene
 * 
 * Features:
 * - 5x5 meter bounded plane with walls
 * - Multiple balls with random initial velocities
 * - Ball-to-ball and ball-to-wall collisions
 * - Performance monitoring
 */
class BallCollisionScene : public BaseScene {
public:
    BallCollisionScene();
    virtual ~BallCollisionScene() = default;

    // BaseScene interface
    const char* getName() const override { return "Ball Collision Scene"; }
    const char* getDescription() const override { return "Multiple balls with collisions on a bounded 5x5m plane"; }
    
    bool initialize(GLFWwindow* window, const SceneConfig& config) override;
    void update(float deltaTime) override;
    void render() override;
    void cleanup() override;

protected:
    // Scene-specific initialization
    void initializeObjects() override;
    void updateScene(float deltaTime) override;
    void renderScene() override;

private:
    // Scene parameters
    static constexpr float PLANE_SIZE = 5.0f;         // 5x5 meters
    static constexpr float BALL_RADIUS = 0.2f;        // 20 cm radius
    static constexpr float WALL_HEIGHT = 0.3f;        // 30 cm height
    static constexpr float WALL_WIDTH = 0.2f;         // 20 cm width
    static constexpr int NUM_BALLS = 15;              // Number of balls (reduced for better spacing)
    
    // Random number generation
    std::random_device m_rd;
    std::mt19937 m_gen;
    std::uniform_real_distribution<float> m_velDist;
    std::uniform_real_distribution<float> m_posDist;

    // Helper functions
    glm::vec3 getRandomVelocity();
    glm::vec3 getRandomPosition();
};
