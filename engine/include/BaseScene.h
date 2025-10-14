#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// Forward declarations
class BulletWorld;
class BulletRigidBody;
class Camera;
class Shader;
class Mesh;
class RigidBody3D;
class FPSRenderer;
class CameraController;

// Include headers for complete type definitions (needed for unique_ptr destructors)
#include "bullet/BulletWorld.h"
#include "bullet/BulletRigidBody.h"
#include "../src/rendering/Camera.h"
#include "../src/rendering/Shader.h"
#include "../src/rendering/Mesh.h"
#include "../src/core/RigidBody3D.h"
#include "../src/rendering/FPSRenderer.h"
#include "rendering/camera/CameraController.h"

/**
 * BaseScene - Base class for all physics scenes
 * 
 * Provides common functionality:
 * - FPS rendering and monitoring
 * - Camera controls
 * - Object creation (Box, Sphere, Plane)
 * - Rendering pipeline
 * - Physics world management
 */
class BaseScene {
public:
    BaseScene();
    virtual ~BaseScene() = default;

    // Scene interface - must be implemented by derived classes
    virtual const char* getName() const = 0;
    virtual const char* getDescription() const = 0;
    
    virtual bool initialize(GLFWwindow* window) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
    
    // FPS display support
    virtual void renderFPS() { if (m_fpsRenderer) m_fpsRenderer->render(getViewMatrix(), getProjectionMatrix()); }
    virtual void toggleFPSDisplay() { if (m_fpsRenderer) m_fpsRenderer->toggleDisplay(); }
    virtual bool isFPSDisplayEnabled() const { return m_fpsRenderer ? m_fpsRenderer->isDisplayEnabled() : false; }
    
    // Camera controller support
    virtual void setCameraController(std::unique_ptr<CameraController> controller);
    virtual CameraController* getCameraController() const { return m_cameraController.get(); }
    virtual void switchToNextCamera();
    virtual void switchToPreviousCamera();
    virtual const std::string& getActiveCameraName() const;
    
    // Static callback for camera switching
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

protected:
    // Core components
    std::unique_ptr<BulletWorld> m_bulletWorld;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<FPSRenderer> m_fpsRenderer;
    std::unique_ptr<CameraController> m_cameraController;
    
    // Meshes
    std::shared_ptr<Mesh> m_boxMesh;
    std::shared_ptr<Mesh> m_sphereMesh;
    std::shared_ptr<Mesh> m_planeMesh;
    
    // Physics objects (raw pointers since they're owned by m_objects)
    std::vector<BulletRigidBody*> m_physicsObjects;
    
    // Window reference
    GLFWwindow* m_window = nullptr;
    
    // Object creation functions
    void createBox(glm::vec3 position, 
                   glm::vec3 scale,
                   glm::vec3 rotation = glm::vec3(0.0f),
                   glm::vec3 color = glm::vec3(0.5f),
                   bool enablePhysics = false,
                   float mass = 1.0f);
                   
    void createSphere(glm::vec3 position, 
                      float radius,
                      glm::vec3 color = glm::vec3(0.5f),
                      bool enablePhysics = false,
                      float mass = 1.0f,
                      glm::vec3 initialVelocity = glm::vec3(0.0f));
                      
    void createPlane(glm::vec3 position, 
                     glm::vec2 size,
                     glm::vec3 rotation = glm::vec3(0.0f),
                     glm::vec3 color = glm::vec3(0.3f),
                     bool enablePhysics = false);
    
    // Rendering functions
    void renderObject(const BulletRigidBody& body, glm::vec3 color);
    void renderAllObjects();
    
    // Matrix getters
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    // Helper functions
    void setupCommonComponents(GLFWwindow* window);
    void setupGLFWCallbacks(GLFWwindow* window);
    void loadCommonMeshes();
    
    // Virtual functions for customization
    virtual void initializeObjects() = 0;
    virtual void updateScene(float deltaTime) {}
    virtual void renderScene() {}
    
    // Object storage
    struct ObjectInfo {
        std::unique_ptr<BulletRigidBody> physicsBody;
        glm::vec3 color;
        std::shared_ptr<Mesh> mesh;
    };
    
    std::vector<ObjectInfo> m_objects;
};
