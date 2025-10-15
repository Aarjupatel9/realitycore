#pragma once

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <functional>

// Forward declaration
struct PhysicsConfig;

/**
 * BulletWorld - Wrapper class for Bullet Physics world
 * 
 * This class provides a clean interface to Bullet Physics' btDiscreteDynamicsWorld
 * and handles all the necessary Bullet Physics components initialization and management.
 * 
 * Features:
 * - Rigid body dynamics simulation
 * - Collision detection and response
 * - Gravity and force application
 * - Collision callbacks
 * - Debug rendering support
 * - Multi-threading support
 */
class BulletWorld {
private:
    // Core Bullet Physics components
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_broadphase;
    btSequentialImpulseConstraintSolver* m_solver;
    btDefaultCollisionConfiguration* m_collisionConfig;
    
    // Collision callback
    std::function<void(btRigidBody*, btRigidBody*)> m_collisionCallback;
    
    // Debug drawing
    bool m_debugDrawEnabled;
    
public:
    /**
     * Constructor with default gravity
     * @param gravity Gravity vector (default: Earth gravity -9.81 m/s²)
     */
    explicit BulletWorld(const glm::vec3& gravity = glm::vec3(0.0f, -9.81f, 0.0f));
    
    /**
     * Constructor with full physics configuration
     * @param config Physics configuration struct
     */
    explicit BulletWorld(const PhysicsConfig& config);
    
    /**
     * Destructor
     */
    ~BulletWorld();
    
    // Disable copy constructor and assignment operator
    BulletWorld(const BulletWorld&) = delete;
    BulletWorld& operator=(const BulletWorld&) = delete;
    
    /**
     * Update the physics simulation
     * @param deltaTime Time step in seconds
     * @param maxSubSteps Maximum number of sub-steps (default: 10)
     * @param fixedTimeStep Fixed time step for sub-steps (default: 1/60)
     */
    void Update(float deltaTime, int maxSubSteps = 10, float fixedTimeStep = 1.0f/60.0f);
    
    /**
     * Add a rigid body to the world
     * @param body Bullet rigid body to add
     */
    void AddRigidBody(btRigidBody* body);
    
    /**
     * Remove a rigid body from the world
     * @param body Bullet rigid body to remove
     */
    void RemoveRigidBody(btRigidBody* body);
    
    /**
     * Set the gravity vector
     * @param gravity New gravity vector
     */
    void SetGravity(const glm::vec3& gravity);
    
    /**
     * Get the current gravity vector
     * @return Current gravity vector
     */
    glm::vec3 GetGravity() const;
    
    /**
     * Set collision callback function
     * @param callback Function to call when collision occurs
     */
    void SetCollisionCallback(std::function<void(btRigidBody*, btRigidBody*)> callback);
    
    /**
     * Enable or disable debug drawing
     * @param enabled True to enable debug drawing
     */
    void SetDebugDrawEnabled(bool enabled);
    
    /**
     * Check if debug drawing is enabled
     * @return True if debug drawing is enabled
     */
    bool IsDebugDrawEnabled() const;
    
    /**
     * Set number of threads for multi-threading
     * @param numThreads Number of threads to use
     */
    void SetNumTasks(int numThreads);
    
    /**
     * Set solver iterations (collision resolution quality)
     * @param iterations Number of solver iterations (higher = more accurate)
     */
    void SetSolverIterations(int iterations);
    
    /**
     * Set error reduction parameter (penetration correction stiffness)
     * @param erp Error reduction parameter (0.0-1.0)
     */
    void SetERP(float erp);
    
    /**
     * Get the Bullet dynamics world (for advanced usage)
     * @return Pointer to btDiscreteDynamicsWorld
     */
    btDiscreteDynamicsWorld* GetDynamicsWorld() const { return m_dynamicsWorld; }
    
    /**
     * Get the Bullet collision dispatcher (for advanced usage)
     * @return Pointer to btCollisionDispatcher
     */
    btCollisionDispatcher* GetCollisionDispatcher() const { return m_dispatcher; }
    
    /**
     * Get the Bullet broadphase (for advanced usage)
     * @return Pointer to btBroadphaseInterface
     */
    btBroadphaseInterface* GetBroadphase() const { return m_broadphase; }
    
private:
    /**
     * Initialize Bullet Physics components
     */
    void InitializeBulletComponents();
    
    /**
     * Initialize with physics configuration
     */
    void InitializeWithConfig(const PhysicsConfig& config);
    
    /**
     * Cleanup Bullet Physics components
     */
    void CleanupBulletComponents();
    
    /**
     * Handle collision detection and callbacks
     */
    void HandleCollisions();
    
    // Store config for debug interval
    int m_debugPrintInterval = 0;
    int m_updateCount = 0;
};
