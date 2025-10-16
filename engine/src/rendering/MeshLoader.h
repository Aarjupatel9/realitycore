#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

// Forward declarations
class Mesh;

/**
 * @brief Generic mesh loader supporting multiple file formats
 * 
 * Supports: .obj, .fbx, .gltf, .glb, .dae, .blend, .3ds, .ply, .stl, and 40+ more formats via Assimp
 * 
 * Usage:
 *   auto mesh = MeshLoader::load("models/car.obj");
 *   auto meshes = MeshLoader::loadWithNormals("models/character.fbx");
 */
class MeshLoader {
public:
    /**
     * @brief Load a mesh from file (positions only)
     * @param filepath Path to mesh file (relative or absolute)
     * @return Shared pointer to loaded mesh, or nullptr on failure
     */
    static std::shared_ptr<Mesh> load(const std::string& filepath);
    
    /**
     * @brief Load a mesh from file (positions + normals)
     * @param filepath Path to mesh file (relative or absolute)
     * @return Shared pointer to loaded mesh, or nullptr on failure
     */
    static std::shared_ptr<Mesh> loadWithNormals(const std::string& filepath);
    
    /**
     * @brief Load a mesh from file (positions + normals + UVs)
     * @param filepath Path to mesh file (relative or absolute)
     * @return Shared pointer to loaded mesh, or nullptr on failure
     */
    static std::shared_ptr<Mesh> loadWithNormalsAndUVs(const std::string& filepath);
    
    /**
     * @brief Load all meshes from a file (for multi-mesh models)
     * @param filepath Path to mesh file (relative or absolute)
     * @return Vector of loaded meshes, or empty vector on failure
     */
    static std::vector<std::shared_ptr<Mesh>> loadAll(const std::string& filepath);
    
    /**
     * @brief Get mesh information without loading
     * @param filepath Path to mesh file
     * @return Mesh statistics (vertex count, triangle count, etc.)
     */
    struct MeshInfo {
        unsigned int vertexCount = 0;
        unsigned int triangleCount = 0;
        unsigned int meshCount = 0;
        bool hasNormals = false;
        bool hasUVs = false;
        bool hasColors = false;
        glm::vec3 boundingBoxMin = glm::vec3(0.0f);
        glm::vec3 boundingBoxMax = glm::vec3(0.0f);
    };
    
    static MeshInfo getMeshInfo(const std::string& filepath);
    
    /**
     * @brief Check if a file format is supported
     * @param filepath Path to check (or just extension like ".obj")
     * @return true if format is supported, false otherwise
     */
    static bool isFormatSupported(const std::string& filepath);
    
    /**
     * @brief Get list of all supported file extensions
     * @return Vector of supported extensions (e.g., "obj", "fbx", "gltf")
     */
    static std::vector<std::string> getSupportedFormats();

private:
    // Private helper methods
    static std::shared_ptr<Mesh> loadMeshFromAssimp(const std::string& filepath, bool includeNormals, bool includeUVs);
    static void extractMeshData(const void* assimpScene, std::vector<std::shared_ptr<Mesh>>& meshes, bool includeNormals, bool includeUVs);
};

