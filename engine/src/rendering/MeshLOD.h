#pragma once

#include <memory>
#include <map>
#include <glm/glm.hpp>

class Mesh;

/**
 * @brief Level of Detail system for meshes
 * 
 * Automatically selects appropriate mesh quality based on distance and object size.
 * Improves performance by using low-poly meshes for distant/small objects.
 */
enum class LODLevel {
    LOW,      // 8×8 segments (64 triangles) - far away or very small
    MEDIUM,   // 16×16 segments (512 triangles) - medium distance
    HIGH,     // 32×32 segments (2048 triangles) - close up
    ULTRA     // 64×64 segments (8192 triangles) - very close or very large
};

class MeshLOD {
public:
    MeshLOD();
    ~MeshLOD() = default;
    
    /**
     * @brief Register a mesh for a specific LOD level
     * @param level LOD level (LOW, MEDIUM, HIGH, ULTRA)
     * @param mesh Mesh to use for this LOD level
     */
    void registerMesh(LODLevel level, std::shared_ptr<Mesh> mesh);
    
    /**
     * @brief Select appropriate mesh based on distance and object size
     * @param distance Distance from camera to object
     * @param objectRadius Approximate size of the object (radius)
     * @return Mesh for the appropriate LOD level
     */
    std::shared_ptr<Mesh> selectMesh(float distance, float objectRadius) const;
    
    /**
     * @brief Get mesh for specific LOD level
     * @param level LOD level to retrieve
     * @return Mesh for the specified LOD, or nullptr if not registered
     */
    std::shared_ptr<Mesh> getMesh(LODLevel level) const;
    
    /**
     * @brief Check if a specific LOD level is registered
     * @param level LOD level to check
     * @return true if mesh is registered for this level, false otherwise
     */
    bool hasMesh(LODLevel level) const;
    
    /**
     * @brief Clear all registered meshes
     */
    void clear();
    
    /**
     * @brief Get the current LOD level that would be selected
     * @param distance Distance from camera to object
     * @param objectRadius Approximate size of the object (radius)
     * @return LOD level that would be selected
     */
    static LODLevel calculateLOD(float distance, float objectRadius);
    
    /**
     * @brief Configure LOD distance thresholds
     * @param lowToMedium Distance at which to switch from LOW to MEDIUM
     * @param mediumToHigh Distance at which to switch from MEDIUM to HIGH
     * @param highToUltra Distance at which to switch from HIGH to ULTRA
     */
    static void setLODThresholds(float lowToMedium, float mediumToHigh, float highToUltra);

private:
    std::map<LODLevel, std::shared_ptr<Mesh>> m_lodMeshes;
    
    // Default distance thresholds (can be configured)
    static float s_lowToMediumThreshold;
    static float s_mediumToHighThreshold;
    static float s_highToUltraThreshold;
};

