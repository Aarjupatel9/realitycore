#include "BallCollisionScene.h"
#include "../../engine/src/rendering/Camera.h"
#include "../../engine/src/core/RigidBody3D.h"
#include "../../engine/src/shapes/Sphere.h"
#include <iostream>

BallCollisionScene::BallCollisionScene() 
    : m_gen(m_rd()),
      m_velDist(-8.0f, 8.0f),
      m_posDist(-PLANE_SIZE/2.0f + BALL_RADIUS, PLANE_SIZE/2.0f - BALL_RADIUS) {
}

bool BallCollisionScene::initialize(GLFWwindow* window, const SceneConfig& config) {
    // Call base class initialization
    if (!BaseScene::initialize(window, config)) {
        return false;
    }
    
    // Set up camera for better viewing of 5x5 plane
    m_camera->setPosition(glm::vec3(0.0f, 8.0f, 8.0f)); // Closer for better view of smaller plane
    m_camera->setYaw(-90.0f); // Look towards center
    m_camera->setPitch(-25.0f); // Look down at the plane
    
    std::cout << "Ball Collision Scene initialized successfully!" << std::endl;
    return true;
}

void BallCollisionScene::initializeObjects() {
    std::cout << "Creating Ball Collision Scene objects..." << std::endl;
    
    // Create floor - just one line!
    createPlane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(PLANE_SIZE, PLANE_SIZE), 
                glm::vec3(0.0f), glm::vec3(0.7f, 0.7f, 0.7f), true);
    
    // Create walls - just one line each!
    createBox(glm::vec3(0.0f, WALL_HEIGHT/2.0f, PLANE_SIZE/2.0f - WALL_WIDTH/2.0f), 
              glm::vec3(PLANE_SIZE, WALL_HEIGHT, WALL_WIDTH), 
              glm::vec3(0.0f), glm::vec3(0.6f, 0.4f, 0.2f), true); // North wall
    
    createBox(glm::vec3(0.0f, WALL_HEIGHT/2.0f, -PLANE_SIZE/2.0f + WALL_WIDTH/2.0f), 
              glm::vec3(PLANE_SIZE, WALL_HEIGHT, WALL_WIDTH), 
              glm::vec3(0.0f), glm::vec3(0.6f, 0.4f, 0.2f), true); // South wall
    
    createBox(glm::vec3(PLANE_SIZE/2.0f - WALL_WIDTH/2.0f, WALL_HEIGHT/2.0f, 0.0f), 
              glm::vec3(WALL_WIDTH, WALL_HEIGHT, 4.6f), 
              glm::vec3(0.0f), glm::vec3(0.6f, 0.4f, 0.2f), true); // East wall
    
    createBox(glm::vec3(-PLANE_SIZE/2.0f + WALL_WIDTH/2.0f, WALL_HEIGHT/2.0f, 0.0f), 
              glm::vec3(WALL_WIDTH, WALL_HEIGHT, 4.6f), 
              glm::vec3(0.0f), glm::vec3(0.6f, 0.4f, 0.2f), true); // West wall
    
    // Create balls - just one line each!
    for (int i = 0; i < NUM_BALLS; i++) {
        glm::vec3 position = getRandomPosition();
        glm::vec3 velocity = getRandomVelocity();
        glm::vec3 color = glm::vec3(0.2f, 0.8f, 0.2f); // Green balls
        createSphere(position, BALL_RADIUS, color, true, 1.0f, velocity); // Physics enabled with velocity
    }
    
    std::cout << "Created " << NUM_BALLS << " balls, 1 floor, and 4 walls" << std::endl;
}

void BallCollisionScene::updateScene(float deltaTime) {
    // No color changes - all objects keep their original colors
    // Physics simulation continues without visual color modifications
}

void BallCollisionScene::renderScene() {
    // Scene-specific rendering (if any)
    // For now, all rendering is handled by BaseScene::renderAllObjects()
}

glm::vec3 BallCollisionScene::getRandomVelocity() {
    return glm::vec3(
        m_velDist(m_gen),
        std::abs(m_velDist(m_gen)) * 0.5f, // Small upward component
        m_velDist(m_gen)
    );
}

glm::vec3 BallCollisionScene::getRandomPosition() {
    return glm::vec3(
        m_posDist(m_gen),
        BALL_RADIUS + 0.1f, // Slightly above ground
        m_posDist(m_gen)
    );
}


void BallCollisionScene::update(float deltaTime) {
    BaseScene::update(deltaTime);
}

void BallCollisionScene::render() {
    BaseScene::render();
}

void BallCollisionScene::cleanup() {
    BaseScene::cleanup();
}
