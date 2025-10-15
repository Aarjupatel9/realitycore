# Engine Hardcoded Values Analysis

## Executive Summary
This document catalogs all hardcoded values, magic numbers, and rigid structures found in the RealityCore engine layer that could benefit from being made configurable or extracted into a configuration system.

---

## 🔴 CRITICAL ISSUES (High Priority)

### 1. **Hardcoded Aspect Ratio (800x600)**
**Location:** `engine/src/BaseScene.cpp:405, 410`
```cpp
return activeCamera->getProjectionMatrix(800.0f/600.0f);  // Line 405
return m_camera ? m_camera->getProjectionMatrix(800.0f/600.0f) : glm::mat4(1.0f);  // Line 410
```
**Impact:** Engine assumes 800x600 window; breaks on different resolutions
**Solution:** Pass window dimensions from scene initialization or get from GLFW

---

### 2. **Hardcoded Light Position and Color**
**Location:** `engine/src/BaseScene.cpp:367-368`
```cpp
m_shader->setUniform("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
m_shader->setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
```
**Impact:** All scenes have same lighting; no control over lighting configuration
**Solution:** Add configurable lighting system with multiple light support

---

### 3. **Hardcoded Background Color (Sky Blue)**
**Location:** `engine/src/BaseScene.cpp:492`
```cpp
glClearColor(0.5f, 0.8f, 1.0f, 1.0f);  // Sky blue hardcoded
```
**Impact:** All scenes have same background; no customization
**Solution:** Make background color configurable per scene

---

### 4. **Hardcoded Camera Initial Values**
**Location:** `engine/src/rendering/Camera.cpp:6-17`
```cpp
Camera::Camera() 
    : m_position(0.0f, 0.0f, 5.0f)      // Hardcoded start position
    , m_yaw(-90.0f)                      // Hardcoded orientation
    , m_pitch(0.0f)
    , m_fov(45.0f)                       // Hardcoded FOV
    , m_moveSpeed(3.0f)                  // Hardcoded speed
    , m_mouseSensitivity(0.1f)           // Hardcoded sensitivity
    , m_lastMouseX(400.0)                // Assumes 800px window
    , m_lastMouseY(300.0)                // Assumes 600px window
```
**Impact:** Camera always starts at same position; sensitivity fixed
**Solution:** Add camera configuration struct with sensible defaults

---

### 5. **Hardcoded Physics Solver Parameters**
**Location:** `engine/src/bullet/BulletWorld.cpp:40-50`
```cpp
solverInfo.m_numIterations = 50;                            // Hardcoded iterations
solverInfo.m_solverMode = SOLVER_SIMD | SOLVER_RANDMIZE_ORDER | SOLVER_USE_WARMSTARTING;
solverInfo.m_splitImpulse = true;
solverInfo.m_splitImpulsePenetrationThreshold = -0.002f;   // Hardcoded threshold
solverInfo.m_erp = 0.2f;                                   // Hardcoded ERP
solverInfo.m_erp2 = 0.2f;
solverInfo.m_globalCfm = 0.0f;
m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;  // Hardcoded CCD
```
**Impact:** Cannot tune physics quality/performance tradeoff per scene
**Solution:** Create PhysicsWorldConfig struct with defaults

---

## 🟡 MODERATE ISSUES (Medium Priority)

### 6. **Hardcoded Camera Movement Keys**
**Location:** `engine/src/rendering/Camera.cpp:35-40`
```cpp
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_position += front * velocity;  // W/S/A/D hardcoded
if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_position -= front * velocity;
if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_position -= right * velocity;
if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_position += right * velocity;
if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) m_position += up * velocity;    // I/K hardcoded
if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) m_position -= up * velocity;
```
**Impact:** Users cannot customize controls
**Solution:** Key binding configuration system

---

### 7. **Hardcoded Sprint Multiplier**
**Location:** `engine/src/rendering/Camera.cpp:32`
```cpp
velocity *= 2.5f; // Sprint - hardcoded multiplier
```
**Impact:** Sprint speed not customizable
**Solution:** Make sprint multiplier configurable

---

### 8. **Hardcoded FOV Limits**
**Location:** `engine/src/rendering/Camera.cpp:44-49`
```cpp
if (m_fov < 20.0f) m_fov = 20.0f;   // Min FOV hardcoded
if (m_fov > 90.0f) m_fov = 90.0f;   // Max FOV hardcoded
```
**Impact:** Cannot have ultra-wide or narrow FOV
**Solution:** Make FOV range configurable

---

### 9. **Hardcoded Grid Renderer Values**
**Location:** `engine/src/rendering/GridRenderer.cpp`
```cpp
GridRenderer::GridRenderer() : ... m_color(0.3f, 0.3f, 0.3f) {}  // Line 6: Gray color
glLineWidth(2.0f);  // Line 33: Line thickness hardcoded
m_vertices.push_back(-halfSize); m_vertices.push_back(0.01f); ...  // Line 52: Grid Y offset 0.01
```
**Impact:** Grid appearance not customizable
**Solution:** Add GridConfig struct

---

### 10. **Hardcoded Ambient Lighting Value**
**Location:** `engine/src/BaseScene.cpp:88` (in shader)
```cpp
float ambient = 0.5;  // Hardcoded in shader source
```
**Impact:** Cannot adjust ambient light intensity
**Solution:** Make ambient a uniform parameter

---

### 11. **Hardcoded Debug Print Frequency**
**Location:** Multiple files
```cpp
// engine/src/BaseScene.cpp:421
if (frameCount % 60 == 0) { ... }  // Debug every 60 frames hardcoded

// engine/src/bullet/BulletWorld.cpp:89
if (updateCount % 60 == 0) { ... }  // Debug every 60 frames hardcoded
```
**Impact:** Debug output frequency fixed at 1 second
**Solution:** Make debug interval configurable or use time-based

---

### 12. **Hardcoded Mesh Segments**
**Location:** `engine/src/core/PhysicsConstants.h:43-44`
```cpp
constexpr int DEFAULT_SPHERE_SEGMENTS = 32;
constexpr int DEFAULT_CYLINDER_SEGMENTS = 16;
```
**Impact:** Cannot adjust mesh quality per scene (LOD)
**Solution:** Already in constants (good!), but should be overridable

---

## 🟢 MINOR ISSUES (Low Priority)

### 13. **Hardcoded Shader Versions**
**Location:** Multiple shader sources in BaseScene, GridRenderer, etc.
```cpp
#version 330 core  // OpenGL 3.3 hardcoded everywhere
```
**Impact:** Cannot use different OpenGL versions
**Solution:** Shader version as configuration (rarely needed)

---

### 14. **Hardcoded Mesh Cache Keys**
**Location:** `engine/src/rendering/MeshCache.h:35-39`
```cpp
static constexpr const char* CUBE_KEY = "cube_1.0";
static constexpr const char* SPHERE_KEY = "sphere_1.0_32_16";
static constexpr const char* GROUND_PLANE_KEY = "ground_plane_1x1";
```
**Impact:** Mesh sizes baked into keys
**Solution:** Dynamic key generation based on parameters

---

### 15. **Hardcoded FPS History Size**
**Location:** `engine/src/rendering/FPSRenderer.h:75`
```cpp
static constexpr size_t HISTORY_SIZE = 60; // 1 second at 60 FPS
```
**Impact:** FPS averaging window fixed
**Solution:** Make configurable (though 60 is reasonable default)

---

### 16. **Hardcoded Grass/Rock Instance Counts**
**Location:** 
- `engine/src/rendering/GrassRenderer.h:27` - `grassCount = 10000`
- `engine/src/rendering/RockRenderer.h:27` - `rockCount = 500`
**Impact:** Performance not tunable per platform
**Solution:** Make instance counts configurable

---

### 17. **Hardcoded Grid Elevation**
**Location:** `engine/src/rendering/GridRenderer.cpp:52-73`
```cpp
m_vertices.push_back(0.01f);  // Y = 0.01 to avoid z-fighting
```
**Impact:** Grid always 1cm above ground
**Solution:** Make grid offset configurable

---

### 18. **Hardcoded Collision Estimation**
**Location:** `engine/src/BaseScene.cpp:475-481`
```cpp
int collisionChecks = objectCount * objectCount / 2;  // N*(N-1)/2 approximation
int trianglesRendered = objectCount * 12;  // Assumes ~12 triangles per object
```
**Impact:** FPS metrics are rough estimates
**Solution:** Get actual values from physics/rendering systems

---

## 📊 SUMMARY BY CATEGORY

### Physics System
- ✅ **Good:** Most physics constants in `PhysicsConstants.h`
- ❌ **Bad:** Bullet solver parameters hardcoded in BulletWorld
- ❌ **Bad:** Gravity hardcoded in BulletWorld constructor (-9.81)

### Rendering System
- ❌ **Bad:** Aspect ratio hardcoded (800/600)
- ❌ **Bad:** Background color hardcoded
- ❌ **Bad:** Light position/color hardcoded
- ❌ **Bad:** Ambient lighting hardcoded in shader source
- ⚠️ **Moderate:** Grid appearance hardcoded

### Camera System
- ❌ **Bad:** Initial position/orientation hardcoded
- ❌ **Bad:** Mouse sensitivity hardcoded
- ⚠️ **Moderate:** Movement keys hardcoded (W/A/S/D)
- ⚠️ **Moderate:** FOV limits hardcoded

### Input System
- ⚠️ **Moderate:** All keyboard bindings hardcoded
- ⚠️ **Moderate:** No rebinding support

---

## 🎯 RECOMMENDED REFACTORING PRIORITIES

### Phase 1: Critical Window/Rendering Issues
1. Fix hardcoded aspect ratio → get from window
2. Make background color configurable
3. Create lighting configuration system

### Phase 2: Camera Improvements
4. Camera configuration struct
5. Make FOV/sensitivity configurable
6. Key binding system

### Phase 3: Physics Tuning
7. PhysicsWorldConfig for solver parameters
8. Make collision estimation accurate

### Phase 4: Polish
9. Dynamic mesh quality (LOD)
10. Configurable debug intervals
11. Input rebinding system

---

## 💡 PROPOSED CONFIGURATION SYSTEM

### SceneConfig Struct
```cpp
struct SceneConfig {
    // Window/Rendering
    unsigned int windowWidth = 800;
    unsigned int windowHeight = 600;
    glm::vec3 backgroundColor = glm::vec3(0.5f, 0.8f, 1.0f);
    
    // Lighting
    glm::vec3 lightPosition = glm::vec3(10.0f, 10.0f, 10.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientIntensity = 0.5f;
    
    // Camera
    glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    float cameraFOV = 45.0f;
    float cameraMoveSpeed = 3.0f;
    float cameraSprintMultiplier = 2.5f;
    float mouseSensitivity = 0.1f;
    
    // Physics
    int solverIterations = 50;
    float physicsFPS = 60.0f;
    bool enableDebugDraw = false;
};
```

### Usage
```cpp
SceneConfig config;
config.windowWidth = 1920;
config.windowHeight = 1080;
config.backgroundColor = glm::vec3(0.1f, 0.1f, 0.15f);  // Dark background

scene->initialize(window, config);
```

---

## 📈 METRICS

- **Total Hardcoded Values Found:** ~50+
- **Critical Issues:** 5
- **Moderate Issues:** 7
- **Minor Issues:** 6
- **Files Affected:** 15+ engine files

---

## ✅ WHAT'S ALREADY GOOD

1. **PhysicsConstants.h** - Good separation of physics defaults
2. **Mesh caching system** - Efficient reuse
3. **BaseScene architecture** - Clean inheritance pattern
4. **Bullet wrapper** - Good abstraction layer

---

## 🚫 ANTI-PATTERNS IDENTIFIED

1. **Magic Numbers in Code** - Values like 2.5f, 0.01f scattered throughout
2. **Shader Constants** - Ambient lighting hardcoded in shader source strings
3. **Assumption-Based Code** - 800x600 assumed everywhere
4. **Debug Code in Production** - Debug prints at fixed intervals

---

## 📝 NOTES

- Window dimensions are currently set in **demo main.cpp files**, not engine
- This is correct separation, but engine needs to **receive** those dimensions
- Many "default" values in PhysicsConstants.h are good, just need to be **overridable**

