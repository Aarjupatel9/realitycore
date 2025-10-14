# 🎥 Multi-Camera System Implementation Plan

**Status**: 🚧 **IN PROGRESS** - Active Development  
**Priority**: High  
**Start Date**: October 2, 2025  
**Estimated Duration**: 3-4 days  

## 📋 **Overview**

Implement a comprehensive multi-camera system that allows scenes to define multiple cameras with different behaviors (Fixed, Follow, Attached, Orbit) and switch between them at runtime using keyboard controls.

## 🎯 **Goals**

### **Primary Objectives**
- ✅ Create flexible camera controller architecture
- ✅ Support multiple camera types (Fixed, Follow, Attached, Orbit)
- ✅ Enable runtime camera switching with smooth transitions
- ✅ Integrate seamlessly with existing BaseScene architecture
- ✅ Provide scene-specific camera configurations

### **Secondary Objectives**
- ✅ Smooth camera transitions and interpolation
- ✅ Configurable camera parameters (speed, smoothness, etc.)
- ✅ Debug information display (active camera name)
- ✅ Extensible architecture for future camera types

## 🏗️ **Architecture Design**

### **Core Components**

```
engine/src/rendering/camera/
├── CameraController.h          # Abstract base interface
├── CameraManager.h/.cpp        # Central camera management
├── FixedCamera.h/.cpp          # Static positioned cameras
├── FollowCamera.h/.cpp         # Object following cameras
├── AttachedCamera.h/.cpp       # Cameras fixed to moving objects
├── OrbitCamera.h/.cpp          # Orbiting cameras
└── InputManager.h/.cpp         # Input handling for cameras
```

### **Class Hierarchy**

```
CameraController (Abstract Base)
├── Scene-Specific Controllers
│   ├── BallCollision2CameraController
│   ├── BasicDemoCameraController
│   └── TerrainCameraController
└── Individual Camera Types
    ├── FixedCamera
    ├── FollowCamera
    ├── AttachedCamera
    └── OrbitCamera
```

## 📅 **Implementation Phases**

## **Phase 1: Core Infrastructure** ⏱️ *2-3 hours*

### **1.1 Create Base Classes**
- [ ] **CameraController.h** - Abstract base interface
- [ ] **CameraManager.h/.cpp** - Central camera management system
- [ ] **InputManager.h/.cpp** - Input handling abstraction

### **1.2 Define Core Interfaces**
```cpp
class CameraController {
public:
    virtual ~CameraController() = default;
    virtual void handleInput(const InputManager& input) = 0;
    virtual void update(float deltaTime) = 0;
    virtual Camera* getActiveCamera() = 0;
    virtual const std::string& getActiveCameraName() const = 0;
    virtual void onActivate() {}
    virtual void onDeactivate() {}
};
```

### **1.3 CameraManager Implementation**
```cpp
class CameraManager {
private:
    std::vector<std::unique_ptr<Camera>> m_cameras;
    std::vector<std::string> m_cameraNames;
    int m_activeCameraIndex = 0;
    
public:
    int addCamera(std::unique_ptr<Camera> camera, const std::string& name);
    void switchToCamera(int index);
    void switchToNextCamera();
    Camera* getActiveCamera();
    const std::string& getActiveCameraName() const;
};
```

## **Phase 2: Camera Types Implementation** ⏱️ *2-3 hours*

### **2.1 FixedCamera**
- [ ] Static positioned cameras
- [ ] Configurable position, target, and up vectors
- [ ] No movement or tracking

### **2.2 FollowCamera**
- [ ] Smooth object following with configurable offset
- [ ] Adjustable smoothness and lag
- [ ] Look-at target with offset support

### **2.3 AttachedCamera**
- [ ] Rigidly attached to moving objects
- [ ] Local position and orientation relative to object
- [ ] Perfect synchronization with object movement

### **2.4 OrbitCamera**
- [ ] Circular orbit around target object
- [ ] Configurable radius, height, and speed
- [ ] Automatic rotation with time

## **Phase 3: BaseScene Integration** ⏱️ *1 hour*

### **3.1 Update BaseScene Class**
```cpp
// Add to BaseScene.h
protected:
    std::unique_ptr<CameraController> m_cameraController;
    
public:
    virtual std::unique_ptr<CameraController> createCameraController() = 0;
```

### **3.2 Scene Initialization Updates**
- [ ] Modify BaseScene constructor to support camera controller
- [ ] Add camera update loop to BaseScene::update()
- [ ] Integrate camera input handling

## **Phase 4: Scene-Specific Implementation** ⏱️ *1-2 hours*

### **4.1 BallCollision2CameraController**
```cpp
class BallCollision2CameraController : public CameraController {
private:
    CameraManager m_cameraManager;
    BulletRigidBody* m_ball1;
    BulletRigidBody* m_ball2;
    BulletRigidBody* m_ground;
    
public:
    BallCollision2CameraController(BulletRigidBody* ball1, BulletRigidBody* ball2, BulletRigidBody* ground);
    void setupCameras();
};
```

### **4.2 Camera Configurations**
- **Camera 1**: Top-down overview (Fixed)
- **Camera 2**: Follow Ball 1 (Follow)
- **Camera 3**: Follow Ball 2 (Follow)
- **Camera 4**: Orbit around scene center (Orbit)

### **4.3 Key Bindings**
- **Key 1-4**: Direct camera selection
- **Tab**: Cycle forward through cameras
- **Shift+Tab**: Cycle backward through cameras

## **Phase 5: Testing & Refinement** ⏱️ *1 hour*

### **5.1 Functionality Testing**
- [ ] Test all camera types individually
- [ ] Verify smooth camera switching
- [ ] Test object reference handling
- [ ] Validate input responsiveness

### **5.2 Performance Testing**
- [ ] Check for memory leaks
- [ ] Verify smooth frame rates
- [ ] Test edge cases (null objects, rapid switching)

### **5.3 User Experience Testing**
- [ ] Intuitive key bindings
- [ ] Smooth transitions
- [ ] Clear visual feedback

## 🔧 **Technical Implementation Details**

### **Camera Controller Interface**
```cpp
// engine/src/rendering/camera/CameraController.h
#pragma once
#include "../Camera.h"
#include "../../core/InputManager.h"

class CameraController {
public:
    virtual ~CameraController() = default;
    
    // Core interface
    virtual void handleInput(const InputManager& input) = 0;
    virtual void update(float deltaTime) = 0;
    virtual Camera* getActiveCamera() = 0;
    virtual const std::string& getActiveCameraName() const = 0;
    
    // Optional callbacks
    virtual void onActivate() {}
    virtual void onDeactivate() {}
};
```

### **CameraManager Implementation**
```cpp
// engine/src/rendering/camera/CameraManager.h
#pragma once
#include "../Camera.h"
#include <vector>
#include <memory>
#include <string>

class CameraManager {
private:
    std::vector<std::unique_ptr<Camera>> m_cameras;
    std::vector<std::string> m_cameraNames;
    int m_activeCameraIndex = 0;
    
public:
    int addCamera(std::unique_ptr<Camera> camera, const std::string& name);
    void removeCamera(int index);
    void switchToCamera(int index);
    void switchToNextCamera();
    void switchToPreviousCamera();
    
    Camera* getActiveCamera();
    const std::string& getActiveCameraName() const;
    int getActiveCameraIndex() const { return m_activeCameraIndex; }
    size_t getCameraCount() const { return m_cameras.size(); }
};
```

### **FollowCamera Example**
```cpp
// engine/src/rendering/camera/FollowCamera.h
#pragma once
#include "../Camera.h"
#include "../../bullet/BulletRigidBody.h"
#include <glm/glm.hpp>

class FollowCamera : public Camera {
private:
    BulletRigidBody* m_targetObject;
    glm::vec3 m_offset;
    glm::vec3 m_lookAtOffset;
    float m_smoothness = 5.0f;
    glm::vec3 m_currentPosition;
    glm::vec3 m_currentTarget;
    
public:
    FollowCamera(BulletRigidBody* target, glm::vec3 offset, glm::vec3 lookAtOffset = glm::vec3(0));
    
    void update(float deltaTime) override;
    void setSmoothness(float smoothness) { m_smoothness = smoothness; }
    void setOffset(glm::vec3 offset) { m_offset = offset; }
    void setLookAtOffset(glm::vec3 offset) { m_lookAtOffset = offset; }
};
```

## 🎮 **Expected User Experience**

### **Camera Controls**
- **Key 1**: Top-down overview camera (Fixed)
- **Key 2**: Follow Ball 1 with smooth tracking (Follow)
- **Key 3**: Follow Ball 2 with smooth tracking (Follow)
- **Key 4**: Orbit around scene center (Orbit)
- **Tab**: Cycle through cameras forward
- **Shift+Tab**: Cycle through cameras backward

### **Visual Feedback**
- Smooth transitions between cameras (no jarring jumps)
- On-screen display of active camera name
- Consistent camera behavior across different scenes

### **Performance Expectations**
- No frame rate impact from camera switching
- Smooth object tracking without stuttering
- Responsive input handling

## 📊 **Success Criteria**

### **Functional Requirements**
- ✅ All 4 camera types implemented and working
- ✅ Smooth switching between cameras
- ✅ Object tracking works correctly
- ✅ Key bindings are responsive
- ✅ Integration with BaseScene is seamless

### **Quality Requirements**
- ✅ No memory leaks or performance issues
- ✅ Code is well-documented and maintainable
- ✅ Architecture is extensible for future camera types
- ✅ User experience is intuitive and smooth

### **Testing Requirements**
- ✅ All camera types tested individually
- ✅ Camera switching tested thoroughly
- ✅ Edge cases handled gracefully
- ✅ Performance impact is minimal

## 🚀 **Future Enhancements**

### **Phase 6: Advanced Features** *(Future)*
- [ ] Camera transition animations (smooth interpolation)
- [ ] Camera shake effects
- [ ] Cinematic camera paths
- [ ] Camera collision detection (prevent clipping through walls)
- [ ] Save/load camera presets
- [ ] Runtime camera parameter adjustment

### **Phase 7: UI Integration** *(Future)*
- [ ] Camera selection UI panel
- [ ] Real-time camera parameter adjustment
- [ ] Camera preview windows
- [ ] Visual camera path indicators

## 📝 **Implementation Notes**

### **Object Reference Handling**
- Use constructor injection for object references
- Store raw pointers (objects managed by BaseScene)
- Add null pointer checks for safety

### **Memory Management**
- CameraManager owns Camera objects via unique_ptr
- Scene-specific controllers manage their own CameraManager
- BaseScene owns the CameraController

### **Input Handling**
- Each scene-specific controller defines its own key bindings
- InputManager provides abstraction over GLFW input
- Support for both direct key selection and cycling

## 🔄 **Integration with Existing Code**

### **BaseScene Changes**
```cpp
// Minimal changes to BaseScene
class BaseScene {
protected:
    std::unique_ptr<CameraController> m_cameraController;
    
public:
    virtual std::unique_ptr<CameraController> createCameraController() = 0;
    
    void update(float deltaTime) {
        // Handle camera updates
        if (m_cameraController) {
            m_cameraController->handleInput(m_input);
            m_cameraController->update(deltaTime);
            m_camera = *m_cameraController->getActiveCamera();
        }
        
        // Continue with existing update logic...
    }
};
```

### **Scene Implementation Example**
```cpp
// BallCollision2Scene.cpp
std::unique_ptr<CameraController> BallCollision2Scene::createCameraController() {
    return std::make_unique<BallCollision2CameraController>(
        m_objects[1].physicsBody,  // ball1
        m_objects[2].physicsBody,  // ball2
        m_objects[0].physicsBody   // ground
    );
}
```

---

**Next Steps**: Start with Phase 1 - Core Infrastructure implementation.
