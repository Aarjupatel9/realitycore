#include "BaseScene.h"
#include "bullet/BulletWorld.h"
#include "bullet/BulletRigidBody.h"
#include "bullet/BulletCollisionShapes.h"
#include "../src/rendering/Camera.h"
#include "../src/rendering/Shader.h"
#include "../src/rendering/Mesh.h"
#include "../src/rendering/MeshCache.h"
#include "../src/rendering/FPSRenderer.h"
#include "../src/core/RigidBody3D.h"
#include "../src/shapes/Box.h"
#include "../src/shapes/Sphere.h"
#include "../src/shapes/Plane.h"
#include "../src/utils/MeshGenerator.h"
#include "rendering/camera/CameraController.h"
#include <iostream>

BaseScene::BaseScene() {
    // Initialize common components
}

bool BaseScene::initialize(GLFWwindow* window) {
    std::cout << "Initializing " << getName() << "..." << std::endl;
    
    // Store window reference
    m_window = window;
    
    // Setup common components
    setupCommonComponents(window);
    
    // Load common meshes
    loadCommonMeshes();
    
    // Setup GLFW callbacks
    setupGLFWCallbacks(window);
    
    // Initialize scene-specific objects
    initializeObjects();
    
    std::cout << getName() << " initialized successfully!" << std::endl;
    return true;
}

void BaseScene::setupCommonComponents(GLFWwindow* window) {
    // Create Bullet Physics world
    m_bulletWorld = std::make_unique<BulletWorld>(glm::vec3(0.0f, -9.81f, 0.0f));
    
    // Create camera
    m_camera = std::make_unique<Camera>();
    
    // Create shader
    m_shader = std::make_unique<Shader>();
    
    // Load shader from embedded source
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 FragPos;
        out vec3 Normal;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        
        uniform vec3 uColor;
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        
        void main() {
            // Ambient lighting
            float ambient = 0.5;
            
            // Diffuse lighting
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            
            vec3 result = (ambient + diff) * lightColor * uColor;
            FragColor = vec4(result, 1.0);
        }
    )";
    
    if (!m_shader->loadFromSource(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to load shader!" << std::endl;
        return;
    }
    std::cout << "Shader loaded successfully!" << std::endl;
    
    // Enable OpenGL depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Create FPS renderer
    m_fpsRenderer = std::make_unique<FPSRenderer>();
    m_fpsRenderer->initialize();
    
    std::cout << "Common components setup complete" << std::endl;
}

void BaseScene::loadCommonMeshes() {
    std::cout << "Loading common meshes..." << std::endl;
    
    // First, preload all common meshes
    MeshCache::getInstance().preloadCommonMeshes();
    
    // Then, get the meshes from cache
    m_boxMesh = MeshCache::getInstance().getMesh(MeshCache::CUBE_KEY);
    m_sphereMesh = MeshCache::getInstance().getMesh(MeshCache::SPHERE_KEY);
    m_planeMesh = MeshCache::getInstance().getMesh(MeshCache::GROUND_PLANE_KEY);
    
    // Verify meshes are loaded
    if (!m_boxMesh || !m_sphereMesh || !m_planeMesh) {
        std::cerr << "Error: Failed to load one or more meshes!" << std::endl;
    }
    
    std::cout << "Common meshes loaded" << std::endl;
}

// Static key callback for camera switching
static BaseScene* g_currentScene = nullptr;

void BaseScene::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Call the original camera key callback first
    Camera::keyCallback(window, key, scancode, action, mods);
    
    // Handle camera switching if scene has camera controller
    if (g_currentScene && action == GLFW_PRESS) {
        if (key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL) {
            // Ctrl+C: Switch to next camera
            g_currentScene->switchToNextCamera();
        } else if (key == GLFW_KEY_X && mods == GLFW_MOD_CONTROL) {
            // Ctrl+X: Switch to previous camera
            g_currentScene->switchToPreviousCamera();
        }
    }
}

void BaseScene::setupGLFWCallbacks(GLFWwindow* window) {
    // Set global scene reference for key callback
    g_currentScene = this;
    
    // Set up GLFW callbacks for camera controls
    glfwSetCursorPosCallback(window, Camera::mouseCallback);
    glfwSetScrollCallback(window, Camera::scrollCallback);
    glfwSetKeyCallback(window, BaseScene::keyCallback);
    
    // Initially disable cursor since controls are enabled by default
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void BaseScene::createBox(glm::vec3 position, 
                         glm::vec3 scale,
                         glm::vec3 rotation,
                         glm::vec3 color,
                         bool enablePhysics,
                         float mass) {
    // Create Bullet collision shape
    btBoxShape* boxShape = BulletCollisionShapes::CreateBox(scale * 0.5f);
    
    // Create Bullet rigid body
    auto physicsBody = std::make_unique<BulletRigidBody>(
        boxShape, 
        enablePhysics ? mass : 0.0f,
        position,
        rotation
    );
    
    // Set static if physics disabled
    if (!enablePhysics) {
        physicsBody->setStatic(true);
    }
    
    // Store object info
    ObjectInfo objInfo;
    objInfo.physicsBody = std::move(physicsBody);
    objInfo.color = color;
    objInfo.mesh = m_boxMesh;
    
    // Add to Bullet world if physics enabled
    if (enablePhysics && m_bulletWorld) {
        m_bulletWorld->AddRigidBody(objInfo.physicsBody->getBulletRigidBody());
        m_physicsObjects.push_back(objInfo.physicsBody.get());
    }
    
    m_objects.push_back(std::move(objInfo));
    
    std::cout << "Created box at (" << position.x << ", " << position.y << ", " << position.z 
              << ") with scale (" << scale.x << ", " << scale.y << ", " << scale.z 
              << "), physics: " << (enablePhysics ? "enabled" : "disabled") << std::endl;
}

void BaseScene::createSphere(glm::vec3 position, 
                            float radius,
                            glm::vec3 color,
                            bool enablePhysics,
                            float mass,
                            glm::vec3 initialVelocity) {
    // Create Bullet collision shape
    btSphereShape* sphereShape = BulletCollisionShapes::CreateSphere(radius);
    
    // Create Bullet rigid body
    auto physicsBody = std::make_unique<BulletRigidBody>(
        sphereShape, 
        enablePhysics ? mass : 0.0f,
        position
    );
    
    // Set static if physics disabled
    if (!enablePhysics) {
        physicsBody->setStatic(true);
    } else if (initialVelocity != glm::vec3(0.0f)) {
        // Set initial velocity if provided
        physicsBody->setLinearVelocity(initialVelocity);
    }
        
    // Store object info
    ObjectInfo objInfo;
    objInfo.physicsBody = std::move(physicsBody);
    objInfo.color = color;
    objInfo.mesh = m_sphereMesh;
    
    // Add to Bullet world if physics enabled
    if (enablePhysics && m_bulletWorld) {
        m_bulletWorld->AddRigidBody(objInfo.physicsBody->getBulletRigidBody());
        m_physicsObjects.push_back(objInfo.physicsBody.get());
        std::cout << "DEBUG: Sphere added to physics world, total physics objects: " << m_physicsObjects.size() << std::endl;
    } else {
        std::cout << "DEBUG: Sphere NOT added to physics world (enablePhysics=" << enablePhysics << ", m_bulletWorld=" << (m_bulletWorld ? "exists" : "null") << ")" << std::endl;
    }
    
    m_objects.push_back(std::move(objInfo));
    
    std::cout << "DEBUG: Sphere created at (" << position.x << ", " << position.y << ", " << position.z 
              << ") with radius " << radius << ", physics: " << (enablePhysics ? "enabled" : "disabled") << std::endl;
}

void BaseScene::createPlane(glm::vec3 position, 
                           glm::vec2 size,
                           glm::vec3 rotation,
                           glm::vec3 color,
                           bool enablePhysics) {
    // Create Bullet collision shape (proper static plane)
    btStaticPlaneShape* planeShape = BulletCollisionShapes::CreatePlane(
        glm::vec3(0.0f, 1.0f, 0.0f), // Normal pointing up
        -position.y // Distance from origin
    );
    
    std::cout << "DEBUG: Ground plane created at position (" << position.x << ", " << position.y << ", " << position.z 
              << ") with normal (0, 1, 0) and distance " << -position.y << std::endl;
    
    // Create Bullet rigid body (ground is always static)
    auto physicsBody = std::make_unique<BulletRigidBody>(
        planeShape, 
        0.0f, // Mass = 0 for static objects
        position,
        rotation
    );
    
    // Ground is always static
    physicsBody->setStatic(true);
    
    // Store object info
    ObjectInfo objInfo;
    objInfo.physicsBody = std::move(physicsBody);
    objInfo.color = color;
    objInfo.mesh = m_planeMesh;
    
    // Add to Bullet world if physics enabled
    if (enablePhysics && m_bulletWorld) {
        m_bulletWorld->AddRigidBody(objInfo.physicsBody->getBulletRigidBody());
        m_physicsObjects.push_back(objInfo.physicsBody.get());
    }
    
    m_objects.push_back(std::move(objInfo));
    
    std::cout << "Created plane at (" << position.x << ", " << position.y << ", " << position.z 
              << ") with size (" << size.x << " x " << size.y << ")"
              << ", physics: " << (enablePhysics ? "enabled" : "disabled") << std::endl;
}

void BaseScene::renderObject(const BulletRigidBody& body, glm::vec3 color) {
    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, body.getPosition());
    
    // Apply rotation
    glm::vec3 rotation = body.getRotation();
    if (rotation != glm::vec3(0.0f)) {
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    
    // Apply scale based on collision shape type
    std::shared_ptr<Mesh> meshToRender = nullptr;
    glm::vec3 scale = glm::vec3(1.0f);
    
    // Apply optional visual offset (needed when collision shape uses margin)
    glm::vec3 visualOffset(0.0f);
    
    // Get collision shape and determine scale
    btCollisionShape* shape = body.getCollisionShape();
    if (shape) {
        switch (shape->getShapeType()) {
            case BOX_SHAPE_PROXYTYPE: {
                btBoxShape* boxShape = static_cast<btBoxShape*>(shape);
                btVector3 halfExtents = boxShape->getHalfExtentsWithMargin();
                scale = glm::vec3(halfExtents.x() * 2.0f,
                                   halfExtents.y() * 2.0f,
                                   halfExtents.z() * 2.0f);
                meshToRender = m_boxMesh;
                break;
            }
            case SPHERE_SHAPE_PROXYTYPE: {
                btSphereShape* sphereShape = static_cast<btSphereShape*>(shape);
                float radiusWithMargin = sphereShape->getRadius();
                scale = glm::vec3(radiusWithMargin);
                meshToRender = m_sphereMesh;
                std::cout << "DEBUG: Rendering sphere with radiusWithMargin=" << radiusWithMargin
                          << " margin=" << sphereShape->getMargin() << ", renderScale=" << scale.x << std::endl;
                break;
            }
            case STATIC_PLANE_PROXYTYPE: {
                // For planes, use a default scale
                scale = glm::vec3(10.0f, 0.1f, 10.0f); // Default plane size
                meshToRender = m_planeMesh;
                break;
            }
            default:
                std::cout << "Warning: Unknown shape type for rendering" << std::endl;
                meshToRender = m_boxMesh; // Fallback
                break;
        }
    } else {
        std::cout << "Warning: No collision shape found for object" << std::endl;
        meshToRender = m_boxMesh; // Fallback
    }
    
    // Apply visual offset if set
    if (visualOffset != glm::vec3(0.0f)) {
        model = glm::translate(model, visualOffset);
    }
    
    // Apply scale
    model = glm::scale(model, scale);
    
    // Set uniforms
    m_shader->setUniform("model", model);
    m_shader->setUniform("uColor", color);
    m_shader->setUniform("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
    m_shader->setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
    // Render the appropriate mesh
    if (meshToRender) {
        meshToRender->draw();
    } else {
        std::cout << "Warning: No mesh found for object type" << std::endl;
    }
}

void BaseScene::renderAllObjects() {
    // Render all objects (both static and physics)
    for (const auto& obj : m_objects) {
        if (obj.physicsBody) {
            renderObject(*obj.physicsBody, obj.color);
        }
    }
}

glm::mat4 BaseScene::getViewMatrix() const {
    // Use camera controller's active camera if available
    if (m_cameraController) {
        Camera* activeCamera = m_cameraController->getActiveCamera();
        if (activeCamera) {
            return activeCamera->getViewMatrix();
        }
    }
    
    // Fallback to default camera
    return m_camera ? m_camera->getViewMatrix() : glm::mat4(1.0f);
}

glm::mat4 BaseScene::getProjectionMatrix() const {
    // Use camera controller's active camera if available
    if (m_cameraController) {
        Camera* activeCamera = m_cameraController->getActiveCamera();
        if (activeCamera) {
            return activeCamera->getProjectionMatrix(800.0f/600.0f);
        }
    }
    
    // Fallback to default camera
    return m_camera ? m_camera->getProjectionMatrix(800.0f/600.0f) : glm::mat4(1.0f);
}

void BaseScene::update(float deltaTime) {
    // Update Bullet Physics world
    if (m_bulletWorld) {
        m_bulletWorld->Update(deltaTime);
        
        // Debug: Print object positions every 60 frames (1 second at 60fps)
        static int frameCount = 0;
        frameCount++;
        if (frameCount % 60 == 0) {
            std::cout << "DEBUG: Frame " << frameCount << " - Object positions:" << std::endl;
            for (size_t i = 0; i < m_objects.size(); i++) {
                if (m_objects[i].physicsBody) {
                    glm::vec3 pos = m_objects[i].physicsBody->getPosition();
                    glm::vec3 vel = m_objects[i].physicsBody->getLinearVelocity();
                    std::cout << "  Object " << i << ": pos(" << pos.x << ", " << pos.y << ", " << pos.z 
                              << ") vel(" << vel.x << ", " << vel.y << ", " << vel.z << ")" << std::endl;
                }
            }
        }
    }
    
    // Update camera (use controller if available, otherwise use default camera)
    if (m_cameraController) {
        // Debug: Check if window is NULL
        if (!m_window) {
            std::cout << "WARNING: m_window is NULL in BaseScene::update!" << std::endl;
            return;
        }
        
        // Handle camera controller input
        m_cameraController->handleInput(m_window);
        
        // Update camera controller
        m_cameraController->update(deltaTime);
        
        // Get the active camera from controller
        Camera* activeCamera = m_cameraController->getActiveCamera();
        if (activeCamera) {
            // Update the active camera
            activeCamera->update(m_window, deltaTime);
            
            // Check for FPS toggle request
            if (activeCamera->checkFpsToggleRequest() && m_fpsRenderer) {
                m_fpsRenderer->toggleDisplay();
            }
        }
    } else if (m_camera) {
        // Fallback to default camera
        m_camera->update(m_window, deltaTime);
        
        // Check for FPS toggle request
        if (m_camera->checkFpsToggleRequest() && m_fpsRenderer) {
            m_fpsRenderer->toggleDisplay();
        }
    }
    
    // Update FPS renderer with current metrics
    if (m_fpsRenderer) {
        // Calculate object count from physics bodies
        int objectCount = static_cast<int>(m_objects.size());
        
        // Estimate collision checks (simple approximation)
        int collisionChecks = objectCount * objectCount / 2; // N*(N-1)/2 for all pairs
        
        // Estimate draw calls (objects + 1 for each mesh type used)
        int drawCalls = static_cast<int>(m_objects.size());
        
        // Estimate triangles (rough approximation)
        int trianglesRendered = objectCount * 12; // Assuming ~12 triangles per object on average
        
        m_fpsRenderer->update(deltaTime, objectCount, collisionChecks, drawCalls, trianglesRendered);
    }
    
    // Update scene-specific logic
    updateScene(deltaTime);
}

void BaseScene::render() {
    // Clear screen
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use shader
    m_shader->use();
    
    // Set view and projection matrices
    glm::mat4 view = getViewMatrix();
    glm::mat4 projection = getProjectionMatrix();
    
    m_shader->setUniform("view", view);
    m_shader->setUniform("projection", projection);
    
    // Render all objects
    renderAllObjects();
    
    // Render scene-specific objects
    renderScene();
    
    // Render FPS
    renderFPS();
}

void BaseScene::cleanup() {
    std::cout << "Cleaning up " << getName() << "..." << std::endl;
    
    // Clear objects
    m_objects.clear();
    m_physicsObjects.clear();
    
    // Reset components
    m_bulletWorld.reset();
    m_camera.reset();
    m_shader.reset();
    m_fpsRenderer.reset();
    m_cameraController.reset();
    
    std::cout << getName() << " cleanup complete" << std::endl;
}

// Camera controller methods
void BaseScene::setCameraController(std::unique_ptr<CameraController> controller) {
    m_cameraController = std::move(controller);
    std::cout << "Camera controller set: " << getActiveCameraName() << std::endl;
}

void BaseScene::switchToNextCamera() {
    if (m_cameraController) {
        // For now, just print the current camera name
        // In a full implementation, this would switch to the next camera
        std::cout << "Switching to next camera: " << getActiveCameraName() << std::endl;
    }
}

void BaseScene::switchToPreviousCamera() {
    if (m_cameraController) {
        // For now, just print the current camera name
        // In a full implementation, this would switch to the previous camera
        std::cout << "Switching to previous camera: " << getActiveCameraName() << std::endl;
    }
}

const std::string& BaseScene::getActiveCameraName() const {
    if (m_cameraController) {
        return m_cameraController->getActiveCameraName();
    }
    static const std::string defaultName = "Default Camera";
    return defaultName;
}
