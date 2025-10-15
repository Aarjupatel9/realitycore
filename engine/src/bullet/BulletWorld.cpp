#include "bullet/BulletWorld.h"
#include "config/PhysicsConfig.h"
#include <iostream>

BulletWorld::BulletWorld(const glm::vec3& gravity) 
    : m_dynamicsWorld(nullptr)
    , m_dispatcher(nullptr)
    , m_broadphase(nullptr)
    , m_solver(nullptr)
    , m_collisionConfig(nullptr)
    , m_debugDrawEnabled(false)
    , m_debugPrintInterval(0)
    , m_updateCount(0)
{
    InitializeBulletComponents();
    SetGravity(gravity);
}

BulletWorld::BulletWorld(const PhysicsConfig& config)
    : m_dynamicsWorld(nullptr)
    , m_dispatcher(nullptr)
    , m_broadphase(nullptr)
    , m_solver(nullptr)
    , m_collisionConfig(nullptr)
    , m_debugDrawEnabled(config.enableDebugDraw)
    , m_debugPrintInterval(config.debugPrintInterval)
    , m_updateCount(0)
{
    InitializeBulletComponents();
    InitializeWithConfig(config);
}

BulletWorld::~BulletWorld() {
    CleanupBulletComponents();
}

void BulletWorld::InitializeBulletComponents() {
    // Create collision configuration
    m_collisionConfig = new btDefaultCollisionConfiguration();
    
    // Create collision dispatcher
    m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
    
    // Create broadphase (spatial partitioning)
    m_broadphase = new btDbvtBroadphase();
    
    // Create constraint solver
    m_solver = new btSequentialImpulseConstraintSolver();
    
    // Create dynamics world
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfig);
    
    // Set default parameters (will be overridden by config if provided)
    m_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    
    // Improve collision resolution with moderate settings
    btContactSolverInfo& solverInfo = m_dynamicsWorld->getSolverInfo();
    solverInfo.m_numIterations = 50;
    solverInfo.m_solverMode = SOLVER_SIMD | SOLVER_RANDMIZE_ORDER | SOLVER_USE_WARMSTARTING;
    solverInfo.m_splitImpulse = true;
    solverInfo.m_splitImpulsePenetrationThreshold = -0.002f;
    solverInfo.m_erp = 0.2f;
    solverInfo.m_erp2 = 0.2f;
    solverInfo.m_globalCfm = 0.0f;
    
    // Set collision margins for better contact detection
    m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;
}

void BulletWorld::InitializeWithConfig(const PhysicsConfig& config) {
    if (!m_dynamicsWorld) {
        std::cerr << "BulletWorld::InitializeWithConfig: Dynamics world not initialized!" << std::endl;
        return;
    }
    
    // Set gravity from config
    SetGravity(config.gravity);
    
    // Configure solver parameters
    btContactSolverInfo& solverInfo = m_dynamicsWorld->getSolverInfo();
    solverInfo.m_numIterations = config.solverIterations;
    solverInfo.m_erp = config.erp;
    solverInfo.m_erp2 = config.erp2;
    solverInfo.m_globalCfm = config.globalCfm;
    solverInfo.m_splitImpulse = config.enableSplitImpulse;
    solverInfo.m_splitImpulsePenetrationThreshold = config.splitImpulseThreshold;
    solverInfo.m_solverMode = SOLVER_SIMD | SOLVER_RANDMIZE_ORDER | SOLVER_USE_WARMSTARTING;
    
    // Set CCD penetration
    m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = config.ccdPenetration;
    
    // Store debug interval
    m_debugPrintInterval = config.debugPrintInterval;
}

void BulletWorld::CleanupBulletComponents() {
    if (m_dynamicsWorld) {
        delete m_dynamicsWorld;
        m_dynamicsWorld = nullptr;
    }
    
    if (m_solver) {
        delete m_solver;
        m_solver = nullptr;
    }
    
    if (m_broadphase) {
        delete m_broadphase;
        m_broadphase = nullptr;
    }
    
    if (m_dispatcher) {
        delete m_dispatcher;
        m_dispatcher = nullptr;
    }
    
    if (m_collisionConfig) {
        delete m_collisionConfig;
        m_collisionConfig = nullptr;
    }
}

void BulletWorld::Update(float deltaTime, int maxSubSteps, float fixedTimeStep) {
    if (!m_dynamicsWorld) {
        std::cerr << "BulletWorld::Update: Dynamics world not initialized!" << std::endl;
        return;
    }
    
    // Debug: Print based on config interval
    m_updateCount++;
    if (m_debugPrintInterval > 0 && m_updateCount % m_debugPrintInterval == 0) {
        std::cout << "DEBUG: BulletWorld::Update called with deltaTime=" << deltaTime 
                  << ", maxSubSteps=" << maxSubSteps << ", fixedTimeStep=" << fixedTimeStep << std::endl;
        
        // Debug: Print all rigid body positions and velocities
        std::cout << "DEBUG: All rigid bodies in world:" << std::endl;
        for (int i = 0; i < m_dynamicsWorld->getNumCollisionObjects(); i++) {
            btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body) {
                btTransform transform = body->getWorldTransform();
                btVector3 pos = transform.getOrigin();
                btVector3 vel = body->getLinearVelocity();
                std::cout << "  Body " << i << ": pos(" << pos.x() << ", " << pos.y() << ", " << pos.z() 
                          << ") vel(" << vel.x() << ", " << vel.y() << ", " << vel.z() 
                          << ") mass=" << body->getInvMass() << " static=" << body->isStaticObject() << std::endl;
                
                // Additional debug for collision shapes
                btCollisionShape* shape = body->getCollisionShape();
                if (shape) {
                    if (shape->getShapeType() == BOX_SHAPE_PROXYTYPE) {
                        btBoxShape* boxShape = static_cast<btBoxShape*>(shape);
                        btVector3 halfExtents = boxShape->getHalfExtentsWithMargin();
                        std::cout << "    Box halfExtents: (" << halfExtents.x() << ", " << halfExtents.y() << ", " << halfExtents.z() << ")" << std::endl;
                        std::cout << "    Box top surface at Y=" << (pos.y() + halfExtents.y()) << std::endl;
                    } else if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE) {
                        btSphereShape* sphereShape = static_cast<btSphereShape*>(shape);
                        float radius = sphereShape->getRadius();
                        std::cout << "    Sphere radius: " << radius << ", bottom at Y=" << (pos.y() - radius) << std::endl;
                    }
                }
            }
        }
    }
    
    // Step the simulation
    m_dynamicsWorld->stepSimulation(deltaTime, maxSubSteps, fixedTimeStep);
    
    // Handle collision callbacks
    HandleCollisions();
}

void BulletWorld::AddRigidBody(btRigidBody* body) {
    if (!m_dynamicsWorld || !body) {
        std::cerr << "BulletWorld::AddRigidBody: Invalid parameters!" << std::endl;
        return;
    }
    
    m_dynamicsWorld->addRigidBody(body);
    std::cout << "DEBUG: RigidBody added to dynamics world. Total bodies: " << m_dynamicsWorld->getNumCollisionObjects() << std::endl;
}

void BulletWorld::RemoveRigidBody(btRigidBody* body) {
    if (!m_dynamicsWorld || !body) {
        std::cerr << "BulletWorld::RemoveRigidBody: Invalid parameters!" << std::endl;
        return;
    }
    
    m_dynamicsWorld->removeRigidBody(body);
}

void BulletWorld::SetGravity(const glm::vec3& gravity) {
    if (!m_dynamicsWorld) {
        std::cerr << "BulletWorld::SetGravity: Dynamics world not initialized!" << std::endl;
        return;
    }
    
    m_dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

glm::vec3 BulletWorld::GetGravity() const {
    if (!m_dynamicsWorld) {
        return glm::vec3(0.0f);
    }
    
    btVector3 gravity = m_dynamicsWorld->getGravity();
    return glm::vec3(gravity.x(), gravity.y(), gravity.z());
}

void BulletWorld::SetCollisionCallback(std::function<void(btRigidBody*, btRigidBody*)> callback) {
    m_collisionCallback = callback;
}

void BulletWorld::SetDebugDrawEnabled(bool enabled) {
    m_debugDrawEnabled = enabled;
}

bool BulletWorld::IsDebugDrawEnabled() const {
    return m_debugDrawEnabled;
}

void BulletWorld::SetNumTasks(int numThreads) {
    if (!m_dynamicsWorld) {
        std::cerr << "BulletWorld::SetNumTasks: Dynamics world not initialized!" << std::endl;
        return;
    }
    
    m_dynamicsWorld->setNumTasks(numThreads);
}

void BulletWorld::SetSolverIterations(int iterations) {
    if (!m_dynamicsWorld) {
        std::cerr << "BulletWorld::SetSolverIterations: Dynamics world not initialized!" << std::endl;
        return;
    }
    
    btContactSolverInfo& solverInfo = m_dynamicsWorld->getSolverInfo();
    solverInfo.m_numIterations = iterations;
}

void BulletWorld::SetERP(float erp) {
    if (!m_dynamicsWorld) {
        std::cerr << "BulletWorld::SetERP: Dynamics world not initialized!" << std::endl;
        return;
    }
    
    btContactSolverInfo& solverInfo = m_dynamicsWorld->getSolverInfo();
    solverInfo.m_erp = erp;
    solverInfo.m_erp2 = erp;  // Set both ERP values
}

void BulletWorld::HandleCollisions() {
    if (!m_collisionCallback || !m_dynamicsWorld) {
        return;
    }
    
    // Get number of manifolds (collision pairs)
    int numManifolds = m_dispatcher->getNumManifolds();
    
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold* contactManifold = m_dispatcher->getManifoldByIndexInternal(i);
        
        // Get the two rigid bodies involved in the collision
        const btCollisionObject* objA = contactManifold->getBody0();
        const btCollisionObject* objB = contactManifold->getBody1();
        
        btRigidBody* bodyA = btRigidBody::upcast(const_cast<btCollisionObject*>(objA));
        btRigidBody* bodyB = btRigidBody::upcast(const_cast<btCollisionObject*>(objB));
        
        if (bodyA && bodyB) {
            // Check if there are contact points
            int numContacts = contactManifold->getNumContacts();
            if (numContacts > 0) {
                // Call the collision callback
                m_collisionCallback(bodyA, bodyB);
            }
        }
    }
}
