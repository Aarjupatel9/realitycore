# Custom Mesh Loading & Advanced Rendering Guide

## Overview

RealityCore now supports loading custom mesh files from various formats and provides advanced rendering features including:

- **Custom Mesh Loading** - Load `.obj`, `.fbx`, `.gltf`, `.blend`, and 40+ other formats
- **LOD (Level of Detail)** - Automatic mesh quality switching based on distance
- **Visual/Collision Decoupling** - Separate high-poly visuals from low-poly physics
- **Compound Shapes** - Complex collision shapes made from multiple primitives
- **GameObject System** - Unity/Unreal-like game object architecture

---

## 1. Custom Mesh Loading

### Supported Formats

RealityCore uses **Assimp** to support 40+ file formats:

- **Common**: `.obj`, `.fbx`, `.gltf`, `.glb`, `.dae` (Collada), `.blend` (Blender)
- **3D Modeling**: `.3ds`, `.max`, `.lwo`, `.lxo`
- **CAD**: `.step`, `.iges`, `.stl`, `.ply`
- **Game Engines**: `.md2`, `.md3`, `.md5mesh`, `.x`
- **And many more...**

### Basic Usage

```cpp
#include "rendering/MeshLoader.h"

// Load a mesh with normals (for lighting)
auto mesh = MeshLoader::loadWithNormals("models/car.obj");

// Load a mesh with normals and UVs (for textures - future support)
auto meshWithUVs = MeshLoader::loadWithNormalsAndUVs("models/character.fbx");

// Load all meshes from a file (for multi-mesh models)
auto meshes = MeshLoader::loadAll("models/scene.gltf");
```

### Get Mesh Information

```cpp
auto info = MeshLoader::getMeshInfo("models/car.obj");

std::cout << "Vertices: " << info.vertexCount << std::endl;
std::cout << "Triangles: " << info.triangleCount << std::endl;
std::cout << "Has Normals: " << info.hasNormals << std::endl;
std::cout << "Has UVs: " << info.hasUVs << std::endl;
std::cout << "Bounding Box: " 
          << info.boundingBoxMin.x << "," << info.boundingBoxMin.y << "," << info.boundingBoxMin.z
          << " to "
          << info.boundingBoxMax.x << "," << info.boundingBoxMax.y << "," << info.boundingBoxMax.z
          << std::endl;
```

### Check Format Support

```cpp
bool supported = MeshLoader::isFormatSupported("car.obj");  // true
auto formats = MeshLoader::getSupportedFormats();  // Returns vector of all supported extensions
```

---

## 2. GameObject System

### What is GameObject?

`GameObject` is a generic container that separates visual representation from physics simulation, similar to Unity/Unreal.

```cpp
struct GameObject {
    // Visual
    std::shared_ptr<Mesh> visualMesh;
    std::shared_ptr<MeshLOD> lodSystem;  // Optional LOD
    glm::vec3 color;
    glm::vec3 visualScale;
    glm::vec3 visualOffset;
    
    // Physics
    std::unique_ptr<BulletRigidBody> rigidBody;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    
    // Metadata
    std::string name;
    std::string tag;
    bool visible;
    bool physicsEnabled;
};
```

### Creating GameObjects in Scenes

```cpp
// Method 1: Load custom mesh from file
GameObject* car = createCustomMeshObject(
    "models/sports_car.obj",        // Mesh file
    glm::vec3(0, 2, 0),             // Position
    glm::vec3(2, 1, 1),             // Scale
    glm::vec3(1.0f, 0.0f, 0.0f),   // Color (red)
    true,                           // Enable physics
    1500.0f                         // Mass (kg)
);

// Method 2: Use primitives (box, sphere)
GameObject* box = createBoxObject(
    glm::vec3(0, 5, 0),             // Position
    glm::vec3(2, 2, 2),             // Size
    glm::vec3(0.5f, 0.5f, 0.5f),   // Color (gray)
    true,                           // Enable physics
    10.0f                           // Mass
);

GameObject* ball = createSphereObject(
    glm::vec3(5, 3, 0),             // Position
    1.0f,                           // Radius
    glm::vec3(0.0f, 1.0f, 0.0f),   // Color (green)
    true,                           // Enable physics
    5.0f                            // Mass
);

// Method 3: Create empty GameObject and customize
GameObject* custom = createGameObject("MyCustomObject");
custom->visualMesh = MeshLoader::load("models/tree.obj");
custom->position = glm::vec3(10, 0, 0);
custom->color = glm::vec3(0.2f, 0.8f, 0.2f);  // Green
```

### Customizing Visual and Collision

```cpp
GameObject* truck = createCustomMeshObject("models/truck.obj", glm::vec3(0,2,0));

// Change collision shape (visual mesh stays the same)
auto compoundShape = CompoundShape::createCarShape(
    glm::vec3(4.0f, 1.5f, 2.0f),  // Body size
    0.4f,                          // Wheel radius
    0.3f,                          // Wheel width
    0.8f                           // Wheel offset
);
// Note: Collision shape management will be added in future updates

// Adjust visual offset (if visual mesh origin doesn't match collision center)
truck->visualOffset = glm::vec3(0, -0.5f, 0);
truck->visualScale = glm::vec3(1.2f, 1.0f, 1.0f);  // Stretch visually
```

---

## 3. LOD (Level of Detail) System

### What is LOD?

LOD automatically switches mesh quality based on distance and object size for optimal performance.

**Example**: A sphere 50 meters away uses 64 triangles, but the same sphere 2 meters away uses 8,192 triangles.

### LOD Levels

| Level | Segments | Triangles | Use Case |
|-------|----------|-----------|----------|
| `LOW` | 8×8 | 64 | Very far away or tiny objects |
| `MEDIUM` | 16×16 | 512 | Medium distance |
| `HIGH` | 32×32 | 2,048 | Close up |
| `ULTRA` | 64×64 | 8,192 | Very close or very large objects |

### Using LOD

```cpp
#include "rendering/MeshLOD.h"

// Create LOD system
auto lodSystem = std::make_shared<MeshLOD>();

// Register meshes for each LOD level
lodSystem->registerMesh(LODLevel::LOW,    MeshCache::getInstance().getMesh("sphere_low"));
lodSystem->registerMesh(LODLevel::MEDIUM, MeshCache::getInstance().getMesh("sphere_medium"));
lodSystem->registerMesh(LODLevel::HIGH,   MeshCache::getInstance().getMesh("sphere_high"));
lodSystem->registerMesh(LODLevel::ULTRA,  MeshCache::getInstance().getMesh("sphere_ultra"));

// Attach to GameObject
GameObject* obj = createGameObject("LOD_Sphere");
obj->lodSystem = lodSystem;
obj->position = glm::vec3(0, 0, 0);
obj->scale = glm::vec3(5.0f);  // 5 meter radius

// LOD will automatically select appropriate mesh based on camera distance
```

### Custom LOD for Loaded Meshes

```cpp
// Load a complex model at different quality levels
auto carLOD = std::make_shared<MeshLOD>();
carLOD->registerMesh(LODLevel::LOW,    MeshLoader::load("models/car_low.obj"));
carLOD->registerMesh(LODLevel::MEDIUM, MeshLoader::load("models/car_medium.obj"));
carLOD->registerMesh(LODLevel::HIGH,   MeshLoader::load("models/car_high.obj"));
carLOD->registerMesh(LODLevel::ULTRA,  MeshLoader::load("models/car_ultra.obj"));

GameObject* car = createGameObject("Sports_Car");
car->lodSystem = carLOD;
```

### Configure LOD Thresholds

```cpp
// Adjust when LOD levels switch
// Default: LOW→MEDIUM at 50, MEDIUM→HIGH at 20, HIGH→ULTRA at 5
MeshLOD::setLODThresholds(
    100.0f,  // LOW to MEDIUM threshold
    40.0f,   // MEDIUM to HIGH threshold
    10.0f    // HIGH to ULTRA threshold
);
```

---

## 4. Compound Collision Shapes

### What are Compound Shapes?

Compound shapes combine multiple primitive shapes (boxes, spheres, cylinders, etc.) into a single collision shape. Perfect for complex objects like cars, characters, or machinery.

### Basic Usage

```cpp
#include "bullet/CompoundShape.h"

CompoundShape builder;

// Car example: body + 4 wheels
builder.addBox(glm::vec3(0,0,0), glm::vec3(4,1,2));            // Body
builder.addSphere(glm::vec3(-1.5,-0.5,1), 0.4f);               // Wheel FL
builder.addSphere(glm::vec3(1.5,-0.5,1), 0.4f);                // Wheel FR
builder.addSphere(glm::vec3(-1.5,-0.5,-1), 0.4f);              // Wheel BL
builder.addSphere(glm::vec3(1.5,-0.5,-1), 0.4f);               // Wheel BR

btCompoundShape* carShape = builder.build();
```

### Pre-built Compound Shapes

```cpp
// Car shape (body + 4 wheels)
auto carShape = CompoundShape::createCarShape(
    glm::vec3(4.0f, 1.5f, 2.0f),  // Body dimensions
    0.4f,                          // Wheel radius
    0.3f,                          // Wheel width
    0.8f                           // Wheel offset from center
);

// Character shape (capsule body + sphere head)
auto characterShape = CompoundShape::createCharacterShape(
    2.0f,  // Total height
    0.5f   // Radius
);
```

### Available Shapes

```cpp
CompoundShape builder;

// Add box (with optional rotation)
builder.addBox(position, size, rotation);

// Add sphere
builder.addSphere(position, radius);

// Add cylinder (with optional rotation)
builder.addCylinder(position, radius, height, rotation);

// Add capsule (with optional rotation)
builder.addCapsule(position, radius, height, rotation);

// Add cone (with optional rotation)
builder.addCone(position, radius, height, rotation);

// Add custom Bullet shape
builder.addCustomShape(position, customShape, rotation);

// Build final compound
btCompoundShape* compound = builder.build();
```

---

## 5. Complete Examples

### Example 1: Sports Car with Custom Mesh

```cpp
void MyScene::initializeObjects() {
    // Load high-poly visual mesh
    GameObject* car = createCustomMeshObject(
        "models/ferrari.obj",
        glm::vec3(0, 1, 0),
        glm::vec3(1, 1, 1),
        glm::vec3(0.8f, 0.1f, 0.1f),  // Red
        true,
        1200.0f  // 1200kg
    );
    
    // The createCustomMeshObject already gives it a simple box collision
    // In future updates, you'll be able to replace it with:
    // auto carCollision = CompoundShape::createCarShape(...);
    // car->rigidBody->setCollisionShape(carCollision);
}
```

### Example 2: Character with LOD

```cpp
void MyScene::initializeObjects() {
    // Create LOD system with different quality meshes
    auto characterLOD = std::make_shared<MeshLOD>();
    characterLOD->registerMesh(LODLevel::LOW,    MeshLoader::load("character_low.obj"));
    characterLOD->registerMesh(LODLevel::MEDIUM, MeshLoader::load("character_med.obj"));
    characterLOD->registerMesh(LODLevel::HIGH,   MeshLoader::load("character_high.obj"));
    
    GameObject* character = createGameObject("Player");
    character->lodSystem = characterLOD;
    character->position = glm::vec3(0, 1, 0);
    character->color = glm::vec3(0.9f, 0.7f, 0.5f);  // Skin tone
    
    // Add character collision (capsule)
    auto capsuleShape = CompoundShape::createCharacterShape(1.8f, 0.3f);
    // Will be integrated in future update
}
```

### Example 3: Scenery Objects (Visual Only)

```cpp
void MyScene::initializeObjects() {
    // Tree (no physics, just visuals)
    GameObject* tree = createGameObject("Oak_Tree");
    tree->visualMesh = MeshLoader::loadWithNormals("models/tree.obj");
    tree->position = glm::vec3(10, 0, 5);
    tree->scale = glm::vec3(3.0f);
    tree->physicsEnabled = false;  // No collision
    
    // Rock (with simple sphere collision)
    GameObject* rock = createCustomMeshObject(
        "models/rock.obj",
        glm::vec3(5, 0, 0),
        glm::vec3(1.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        true,
        100.0f
    );
}
```

---

## 6. Performance Considerations

### Mesh Complexity

| Vertices | Triangles | Use Case | Performance |
|----------|-----------|----------|-------------|
| < 1,000 | < 500 | Simple objects, distant objects | Excellent |
| 1,000 - 10,000 | 500 - 5,000 | Medium detail, mid-range | Good |
| 10,000 - 50,000 | 5,000 - 25,000 | High detail, close-up | Moderate |
| > 50,000 | > 25,000 | Ultra detail, hero objects only | Use LOD! |

### Best Practices

1. **Use LOD for Everything** - Even simple objects benefit from LOD at extreme distances
2. **Separate Visual from Collision** - Use high-poly visual, low-poly collision
3. **Compound Shapes Over Mesh Colliders** - Much faster than mesh-based collision
4. **Batch Similar Objects** - Use instancing for grass, rocks, etc. (already implemented)
5. **Profile First** - Use FPS counter to identify bottlenecks

---

## 7. Future Enhancements (Planned)

- **Texture Support** - Load and apply textures from mesh files
- **Material System** - PBR materials (metallic, roughness, etc.)
- **Skeletal Animation** - Load and play animated meshes
- **Mesh Colliders** - Use actual mesh geometry for collision (convex hull)
- **Instanced Rendering** - Efficient rendering of many identical objects
- **Occlusion Culling** - Don't render what you can't see

---

## 8. API Reference

### MeshLoader

```cpp
class MeshLoader {
    // Load mesh (positions only)
    static std::shared_ptr<Mesh> load(const std::string& filepath);
    
    // Load mesh (positions + normals)
    static std::shared_ptr<Mesh> loadWithNormals(const std::string& filepath);
    
    // Load mesh (positions + normals + UVs)
    static std::shared_ptr<Mesh> loadWithNormalsAndUVs(const std::string& filepath);
    
    // Load all meshes from file
    static std::vector<std::shared_ptr<Mesh>> loadAll(const std::string& filepath);
    
    // Get mesh info without loading
    static MeshInfo getMeshInfo(const std::string& filepath);
    
    // Check format support
    static bool isFormatSupported(const std::string& filepath);
    static std::vector<std::string> getSupportedFormats();
};
```

### MeshLOD

```cpp
class MeshLOD {
    void registerMesh(LODLevel level, std::shared_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> selectMesh(float distance, float objectRadius) const;
    std::shared_ptr<Mesh> getMesh(LODLevel level) const;
    bool hasMesh(LODLevel level) const;
    void clear();
    
    static LODLevel calculateLOD(float distance, float objectRadius);
    static void setLODThresholds(float lowToMedium, float mediumToHigh, float highToUltra);
};
```

### CompoundShape

```cpp
class CompoundShape {
    void addBox(const glm::vec3& position, const glm::vec3& size, const glm::quat& rotation = ...);
    void addSphere(const glm::vec3& position, float radius);
    void addCylinder(const glm::vec3& position, float radius, float height, const glm::quat& rotation = ...);
    void addCapsule(const glm::vec3& position, float radius, float height, const glm::quat& rotation = ...);
    void addCone(const glm::vec3& position, float radius, float height, const glm::quat& rotation = ...);
    void addCustomShape(const glm::vec3& position, btCollisionShape* shape, const glm::quat& rotation = ...);
    
    btCompoundShape* build();
    int getShapeCount() const;
    void clear();
    
    static btCompoundShape* createCarShape(const glm::vec3& bodySize, float wheelRadius, float wheelWidth, float wheelOffset);
    static btCompoundShape* createCharacterShape(float height, float radius);
};
```

### BaseScene GameObject Methods

```cpp
// Create empty GameObject
GameObject* createGameObject(const std::string& name = "GameObject");

// Create with custom mesh from file
GameObject* createCustomMeshObject(
    const std::string& meshPath,
    glm::vec3 position,
    glm::vec3 scale = glm::vec3(1.0f),
    glm::vec3 color = glm::vec3(1.0f),
    bool enablePhysics = false,
    float mass = 1.0f
);

// Create primitive GameObjects
GameObject* createBoxObject(glm::vec3 position, glm::vec3 size, glm::vec3 color, bool enablePhysics, float mass);
GameObject* createSphereObject(glm::vec3 position, float radius, glm::vec3 color, bool enablePhysics, float mass);

// Render GameObject
void renderGameObject(const GameObject& gameObject);
```

---

## Conclusion

RealityCore now offers professional-grade mesh loading and rendering capabilities:

✅ Load meshes from 40+ file formats  
✅ Automatic LOD system for performance  
✅ Decouple visual meshes from collision shapes  
✅ Build complex collision shapes easily  
✅ Unity/Unreal-like GameObject architecture  

**Happy coding!** 🚀

