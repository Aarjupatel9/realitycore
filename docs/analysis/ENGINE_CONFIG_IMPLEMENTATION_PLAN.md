# Engine Configuration System - Implementation Plan

## Overview
Implement a comprehensive configuration system using config structs + setter functions to eliminate all hardcoded values from the engine layer.

---

## 🎯 Design Principles

1. **Config structs have sensible defaults** - Zero config needed for basic usage
2. **Setters allow runtime changes** - Dynamic reconfiguration when needed
3. **No backward compatibility** - Clean break, update all demos
4. **Organized by subsystem** - Rendering, Physics, Camera, Input
5. **Window-aware** - Get actual window dimensions, not assume 800x600

---

## 📁 File Structure

```
engine/
├── include/
│   └── config/
│       ├── SceneConfig.h         (Main config aggregator)
│       ├── RenderConfig.h        (Rendering settings)
│       ├── CameraConfig.h        (Camera settings)
│       ├── PhysicsConfig.h       (Physics solver settings)
│       └── InputConfig.h         (Key bindings - future)
└── src/
    └── config/
        └── (implementation files if needed)
```

---

## 📋 Configuration Structures

### **1. RenderConfig.h**
```cpp
#pragma once
#include <glm/glm.hpp>

struct RenderConfig {
    // Background
    glm::vec3 backgroundColor = glm::vec3(0.5f, 0.8f, 1.0f);  // Sky blue
    
    // Lighting
    glm::vec3 lightPosition = glm::vec3(10.0f, 10.0f, 10.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);       // White light
    float ambientIntensity = 0.5f;                             // 50% ambient
    
    // Grid (if used)
    glm::vec3 gridColor = glm::vec3(0.3f, 0.3f, 0.3f);        // Gray
    float gridLineWidth = 2.0f;
    float gridElevation = 0.01f;                               // Above ground to avoid z-fighting
    
    // Shader
    const char* shaderVersion = "#version 330 core";           // OpenGL 3.3
};
```

### **2. CameraConfig.h**
```cpp
#pragma once
#include <glm/glm.hpp>

struct CameraConfig {
    // Initial state
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
    float yaw = -90.0f;                                        // Looking forward
    float pitch = 0.0f;                                        // Level
    
    // View parameters
    float fov = 45.0f;                                         // Field of view (degrees)
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    
    // Movement
    float moveSpeed = 3.0f;                                    // m/s
    float sprintMultiplier = 2.5f;                             // Sprint speed boost
    float mouseSensitivity = 0.1f;
    
    // FOV limits (for zoom)
    float minFOV = 20.0f;
    float maxFOV = 90.0f;
    
    // Controls
    bool controlsEnabled = true;
};
```

### **3. PhysicsConfig.h**
```cpp
#pragma once
#include <glm/glm.hpp>

struct PhysicsConfig {
    // Gravity
    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);        // Earth gravity
    
    // Bullet solver parameters
    int solverIterations = 50;                                 // Collision resolution quality
    float erp = 0.2f;                                          // Error reduction parameter (0-1)
    float erp2 = 0.2f;                                         // ERP for contact constraints
    float globalCfm = 0.0f;                                    // Constraint force mixing
    
    // Split impulse (prevents sinking)
    bool enableSplitImpulse = true;
    float splitImpulseThreshold = -0.002f;
    
    // Continuous collision detection
    float ccdPenetration = 0.0001f;                            // Allowed penetration
    
    // Simulation
    float fixedTimeStep = 1.0f / 60.0f;                        // 60 FPS physics
    int maxSubSteps = 10;                                      // Max substeps per frame
    
    // Debug
    bool enableDebugDraw = false;
    int debugPrintInterval = 60;                               // Frames between debug prints (0 = off)
};
```

### **4. SceneConfig.h (Main aggregator)**
```cpp
#pragma once
#include "RenderConfig.h"
#include "CameraConfig.h"
#include "PhysicsConfig.h"

struct SceneConfig {
    RenderConfig rendering;
    CameraConfig camera;
    PhysicsConfig physics;
    
    // Window dimensions (passed from main, not defaults)
    unsigned int windowWidth = 0;   // Must be set from actual window
    unsigned int windowHeight = 0;  // Must be set from actual window
    
    // Helper to get aspect ratio
    float getAspectRatio() const {
        if (windowHeight == 0) return 800.0f / 600.0f;  // Fallback
        return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    }
};
```

---

## 🔧 BaseScene Changes

### **Updated BaseScene.h**
```cpp
class BaseScene {
public:
    BaseScene();
    virtual ~BaseScene() = default;

    // Initialize with config
    virtual bool initialize(GLFWwindow* window, const SceneConfig& config);
    
    // Setters for runtime changes
    void setBackgroundColor(const glm::vec3& color);
    void setLightPosition(const glm::vec3& pos);
    void setLightColor(const glm::vec3& color);
    void setAmbientIntensity(float intensity);
    void setCameraPosition(const glm::vec3& pos);
    void setCameraFOV(float fov);
    void setPhysicsSolverIterations(int iterations);
    
    // Getters for config
    const SceneConfig& getConfig() const { return m_config; }
    SceneConfig& getConfig() { return m_config; }

protected:
    SceneConfig m_config;  // Scene configuration
    // ... existing members
};
```

### **Key Implementation Changes:**

1. **Store config in BaseScene**
```cpp
SceneConfig m_config;
```

2. **Use config values instead of hardcoded**
```cpp
// OLD:
glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

// NEW:
glClearColor(m_config.rendering.backgroundColor.r, 
             m_config.rendering.backgroundColor.g, 
             m_config.rendering.backgroundColor.b, 
             1.0f);
```

3. **Fix aspect ratio**
```cpp
// OLD:
return activeCamera->getProjectionMatrix(800.0f/600.0f);

// NEW:
return activeCamera->getProjectionMatrix(m_config.getAspectRatio());
```

4. **Pass config to Camera**
```cpp
// In BaseScene::setupCommonComponents()
m_camera = std::make_unique<Camera>(m_config.camera);
```

5. **Pass config to BulletWorld**
```cpp
m_bulletWorld = std::make_unique<BulletWorld>(m_config.physics);
```

---

## 📝 Demo Changes (Example)

### **Before:**
```cpp
int main() {
    GLFWwindow* window = glfwCreateWindow(800, 600, "Demo", nullptr, nullptr);
    
    auto scene = std::make_unique<MyScene>();
    scene->initialize(window);
    // ... run loop
}
```

### **After:**
```cpp
int main() {
    const int WIDTH = 1920;
    const int HEIGHT = 1080;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Demo", nullptr, nullptr);
    
    // Create config (use defaults or customize)
    SceneConfig config;
    config.windowWidth = WIDTH;
    config.windowHeight = HEIGHT;
    
    // Optional: customize for this demo
    config.rendering.backgroundColor = glm::vec3(0.1f, 0.1f, 0.15f);  // Dark background
    config.camera.position = glm::vec3(0.0f, 5.0f, 10.0f);            // Higher camera
    config.camera.fov = 60.0f;                                         // Wider FOV
    config.physics.solverIterations = 100;                             // High precision
    
    auto scene = std::make_unique<MyScene>();
    scene->initialize(window, config);
    // ... run loop
}
```

---

## 🔄 Migration Steps (What I'll Do)

### **Step 1: Create Config Headers**
- Create `engine/include/config/` directory
- Add RenderConfig.h
- Add CameraConfig.h  
- Add PhysicsConfig.h
- Add SceneConfig.h (aggregator)

### **Step 2: Update Core Classes**
- **Camera**: Add constructor accepting CameraConfig
- **BulletWorld**: Add constructor accepting PhysicsConfig
- **BaseScene**: Add initialize(window, config), store m_config

### **Step 3: Replace Hardcoded Values**
- BaseScene.cpp: Use m_config instead of literals
- Camera.cpp: Use config instead of hardcoded defaults
- BulletWorld.cpp: Use config instead of hardcoded solver params

### **Step 4: Add Setter Functions**
- BaseScene: Add setBackgroundColor(), setLightPosition(), etc.
- Setters update m_config and apply changes immediately

### **Step 5: Update All Demos**
- Update 6 demo main.cpp files to pass config
- Extract window dimensions and pass to config
- Test each demo after changes

### **Step 6: Add Aspect Ratio Helper**
- Get actual window dimensions in BaseScene
- Use getAspectRatio() everywhere instead of 800/600

---

## 📊 Estimated Changes

- **New files:** 4 config headers
- **Modified engine files:** ~8 (BaseScene, Camera, BulletWorld, etc.)
- **Modified demo files:** ~6 (all demo main.cpp files)
- **Lines of code:** ~300 changes total
- **Breaking changes:** Yes (all demos need config)
- **Backward compatibility:** No (clean break)

---

## ✅ Benefits After Implementation

1. **No more magic numbers** - All values visible and documented
2. **Easy customization** - Change config, not code
3. **Consistent API** - Same pattern across all systems
4. **Better testing** - Can test with different configs
5. **Future-proof** - Easy to add new config options
6. **Self-documenting** - Config struct shows what's configurable

---

## 🎯 Ready to Proceed?

If you approve this plan, I'll start with:
1. ✅ Create the 4 config header files
2. ✅ Update BaseScene to accept and use config
3. ✅ Update Camera and BulletWorld constructors
4. ✅ Replace all hardcoded values
5. ✅ Add setter functions
6. ✅ Update all 6 demos
7. ✅ Test each demo
8. ✅ Commit changes

**Say "go ahead" and I'll implement this!** 🚀

---

## 📌 Notes

- This will touch ~15 files total
- All demos will need updates (6 files)
- Breaking change: demos won't compile until updated
- Estimated time: ~30-40 tool calls
- Will test after each major step
- Can iterate on config structure if needed

**No code changes yet - waiting for your approval!** ✋

