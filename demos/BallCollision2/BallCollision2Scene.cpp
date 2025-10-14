#include "BallCollision2Scene.h"
#include "BallCollision2CameraController.h"
#include <iostream>

BallCollision2Scene::BallCollision2Scene() {
    std::cout << "=== BallCollision2 Scene ===" << std::endl;
    std::cout << "Starting BallCollision2 Scene..." << std::endl;
}

bool BallCollision2Scene::initialize(GLFWwindow* window) {
    std::cout << "Initializing BallCollision2 Scene..." << std::endl;
    
    // Call base class initialization (sets up camera, shaders, etc.)
    // Note: BaseScene::initialize() already calls initializeObjects()
    if (!BaseScene::initialize(window)) {
        return false;
    }
    
    // Store references to key objects for camera controller
    m_ball1 = nullptr;
    m_ball2 = nullptr;
    m_ground = nullptr;
    
    std::cout << "BallCollision2 Scene initialized successfully!" << std::endl;
    return true;
}

void BallCollision2Scene::initializeObjects() {
    std::cout << "Creating BallCollision2 Scene objects..." << std::endl;
    
    // Create ground box: 5x5x0.3 meters, static (mass = 0, no gravity effect)
    // Position at Y = 0.15 so the top surface is at Y = 0.3 and bottom at Y = 0
    createBox(glm::vec3(0.0f, 0.15f, 0.0f),     // position (center of box)
              glm::vec3(5.0f, 0.3f, 5.0f),      // scale (5x5x0.3)
              glm::vec3(0.0f),                   // rotation
              glm::vec3(0.3f, 0.3f, 0.3f),      // gray color
              true,                              // enable physics
              0.0f);                             // mass = 0 (static, won't fall)
    
    // Store reference to ground (first object created)
    m_ground = m_objects.empty() ? nullptr : m_objects[0].physicsBody.get();
    
    // Create boundary walls around the ground perimeter
    // Wall height: 0.3m, Wall width: 0.3m
    // Ground extends from -2.5 to +2.5 in both X and Z directions
    
    // North Wall (positive Z edge)
    createBox(glm::vec3(0.0f, 0.45f, 2.65f),    // position (Y = ground_top + wall_height/2 = 0.3 + 0.15)
              glm::vec3(5.0f, 0.3f, 0.3f),      // scale (5m length, 0.3m height, 0.3m width)
              glm::vec3(0.0f),                   // rotation
              glm::vec3(0.8f, 0.5f, 0.2f),      // orange color
              true,                              // enable physics
              0.0f);                             // mass = 0 (static)
    
    // South Wall (negative Z edge)
    createBox(glm::vec3(0.0f, 0.45f, -2.65f),   // position
              glm::vec3(5.0f, 0.3f, 0.3f),      // scale
              glm::vec3(0.0f),                   // rotation
              glm::vec3(0.8f, 0.5f, 0.2f),      // orange color
              true,                              // enable physics
              0.0f);                             // mass = 0 (static)
    
    // East Wall (positive X edge)
    createBox(glm::vec3(2.65f, 0.45f, 0.0f),    // position
              glm::vec3(0.3f, 0.3f, 5.0f),      // scale (0.3m width, 0.3m height, 5m length)
              glm::vec3(0.0f),                   // rotation
              glm::vec3(0.8f, 0.5f, 0.2f),      // orange color
              true,                              // enable physics
              0.0f);                             // mass = 0 (static)
    
    // West Wall (negative X edge)
    createBox(glm::vec3(-2.65f, 0.45f, 0.0f),   // position
              glm::vec3(0.3f, 0.3f, 5.0f),      // scale
              glm::vec3(0.0f),                   // rotation
              glm::vec3(0.8f, 0.5f, 0.2f),      // orange color
              true,                              // enable physics
              0.0f);                             // mass = 0 (static)
    
    // Create ball: radius = 0.3, placed on ground surface with random initial velocity
    // Generate random direction - change the angle below for different bouncing patterns:
    // Interesting angles to try: 15°, 35°, 67°, 123°, 200°, 285°, 330°
    float randomAngle = 67.0f; // degrees (change this for different directions)
    float speed = 1.39f; // 5 km/h = 1.39 m/s
    float radians = glm::radians(randomAngle);
    glm::vec3 randomVelocity = glm::vec3(
        speed * cos(radians),  // X component
        0.0f,                  // Y component (no vertical movement)
        speed * sin(radians)   // Z component
    );
    
    createSphere(glm::vec3(0.0f, 0.6f, 0.0f),   // position (ground_top + radius = 0.3 + 0.3 = 0.6)
                 0.3f,                           // radius = 0.3
                 glm::vec3(1.0f, 0.0f, 0.0f),   // red color
                 true,                           // enable physics
                 1.0f,                           // mass = 1.0 (dynamic)
                 randomVelocity);                // initial velocity: random direction
    
    // Store reference to first ball (6th object: ground + 4 walls + 1st ball)
    m_ball1 = m_objects.size() >= 6 ? m_objects[5].physicsBody.get() : nullptr;
    
    // Create second ball with different angle for ball-to-ball collisions
    float secondAngle = 200.0f; // Different angle for interesting collisions
    float secondRadians = glm::radians(secondAngle);
    glm::vec3 secondVelocity = glm::vec3(
        speed * cos(secondRadians),  // X component
        0.0f,                        // Y component (no vertical movement)
        speed * sin(secondRadians)   // Z component
    );
    
    createSphere(glm::vec3(-1.5f, 0.6f, -1.5f), // position offset from first ball
                 0.3f,                           // radius = 0.3
                 glm::vec3(0.0f, 1.0f, 0.0f),   // green color (different from first ball)
                 true,                           // enable physics
                 1.0f,                           // mass = 1.0 (dynamic)
                 secondVelocity);                // initial velocity: different random direction
    
    // Store reference to second ball (7th object: ground + 4 walls + 2 balls)
    m_ball2 = m_objects.size() >= 7 ? m_objects[6].physicsBody.get() : nullptr;
    
    // Make all objects frictionless with perfect bounce for endless ball movement
    for (auto& object : m_objects) {
        if (object.physicsBody) {
            btRigidBody* bulletBody = object.physicsBody->getBulletRigidBody();
            if (bulletBody) {
                bulletBody->setFriction(0.0f);        // No surface friction (frictionless)
                bulletBody->setRollingFriction(0.0f); // No rolling friction (endless movement)
                bulletBody->setRestitution(1.0f);     // Perfect bounce (no energy loss)
                
                // For the ball (last object), add additional settings
                if (&object == &m_objects.back()) {
                    // Prevent ball from sleeping to ensure endless movement
                    bulletBody->setActivationState(DISABLE_DEACTIVATION);
                    
                    // Reduce collision margin for better precision
                    if (bulletBody->getCollisionShape()) {
                        bulletBody->getCollisionShape()->setMargin(0.0001f);
                    }
                }
            }
        }
    }
    
    std::cout << "BallCollision2 Scene: Created ground box (5x5x0.3), 4 boundary walls (0.3x0.3), and ball (r=0.3)" << std::endl;
    std::cout << "Ground: center at (0,0.15,0), top surface at Y=0.3" << std::endl;
    std::cout << "Walls: height=0.3m, width=0.3m, positioned at ground edges" << std::endl;
    std::cout << "Ball 1 (Red): placed at (0,0.6,0) with velocity 1.39 m/s at " << randomAngle << "° angle" << std::endl;
    std::cout << "Ball 1 velocity: (" << randomVelocity.x << ", " << randomVelocity.y << ", " << randomVelocity.z << ")" << std::endl;
    std::cout << "Ball 2 (Green): placed at (-1.5,0.6,-1.5) with velocity 1.39 m/s at " << secondAngle << "° angle" << std::endl;
    std::cout << "Ball 2 velocity: (" << secondVelocity.x << ", " << secondVelocity.y << ", " << secondVelocity.z << ")" << std::endl;
    std::cout << "All objects: friction=0.0, rolling=0.0, restitution=1.0 (perfectly elastic environment)" << std::endl;
    std::cout << "Ball settings: never sleeps, reduced collision margin for precision" << std::endl;
    
    // Setup camera controller with multiple camera views
    std::cout << "Setting up camera controller..." << std::endl;
    auto cameraController = std::make_unique<BallCollision2CameraController>(m_ball1, m_ball2, m_ground);
    setCameraController(std::move(cameraController));
    std::cout << "Camera controller setup complete. Use Ctrl+C and Ctrl+X to switch cameras." << std::endl;
}

void BallCollision2Scene::update(float deltaTime) {
    // Call base class update (handles physics, camera, etc.)
    BaseScene::update(deltaTime);
    
    // Debug: Track both balls' velocity every 60 frames (about once per second at 60 FPS)
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 60 == 0 && m_objects.size() >= 2) {
        // Track Ball 1 (Red) - second to last object (ground, walls, ball1, ball2)
        auto& ball1Object = m_objects[m_objects.size() - 2];
        if (ball1Object.physicsBody) {
            glm::vec3 pos1 = ball1Object.physicsBody->getPosition();
            glm::vec3 vel1 = ball1Object.physicsBody->getLinearVelocity();
            float speed1 = glm::length(vel1);
            std::cout << "BALL 1 (Red): Frame " << frameCount << " - Pos(" << pos1.x << ", " << pos1.y << ", " << pos1.z 
                      << ") Vel(" << vel1.x << ", " << vel1.y << ", " << vel1.z << ") Speed=" << speed1 << std::endl;
        }
        
        // Track Ball 2 (Green) - last object
        auto& ball2Object = m_objects.back();
        if (ball2Object.physicsBody) {
            glm::vec3 pos2 = ball2Object.physicsBody->getPosition();
            glm::vec3 vel2 = ball2Object.physicsBody->getLinearVelocity();
            float speed2 = glm::length(vel2);
            std::cout << "BALL 2 (Green): Frame " << frameCount << " - Pos(" << pos2.x << ", " << pos2.y << ", " << pos2.z 
                      << ") Vel(" << vel2.x << ", " << vel2.y << ", " << vel2.z << ") Speed=" << speed2 << std::endl;
        }
        
        std::cout << "---" << std::endl; // Separator for readability
    }
    
    // Add your custom update logic here
}

void BallCollision2Scene::render() {
    // Call base class render (handles all rendering)
    BaseScene::render();
}

void BallCollision2Scene::cleanup() {
    std::cout << "Cleaning up BallCollision2 Scene..." << std::endl;
    BaseScene::cleanup();
    std::cout << "BallCollision2 Scene cleanup complete" << std::endl;
}
