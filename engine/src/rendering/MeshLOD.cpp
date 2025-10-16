#include "MeshLOD.h"
#include "Mesh.h"
#include <iostream>

// Initialize static thresholds (distance × radius product)
float MeshLOD::s_lowToMediumThreshold = 50.0f;
float MeshLOD::s_mediumToHighThreshold = 20.0f;
float MeshLOD::s_highToUltraThreshold = 5.0f;

MeshLOD::MeshLOD() {
    // Default constructor
}

void MeshLOD::registerMesh(LODLevel level, std::shared_ptr<Mesh> mesh) {
    if (!mesh) {
        std::cerr << "MeshLOD WARNING: Attempted to register null mesh" << std::endl;
        return;
    }
    m_lodMeshes[level] = mesh;
}

std::shared_ptr<Mesh> MeshLOD::selectMesh(float distance, float objectRadius) const {
    LODLevel level = calculateLOD(distance, objectRadius);
    
    // Try to get the calculated LOD level
    auto it = m_lodMeshes.find(level);
    if (it != m_lodMeshes.end()) {
        return it->second;
    }
    
    // Fallback: try to find any available mesh (prefer higher quality)
    const LODLevel fallbackOrder[] = { LODLevel::HIGH, LODLevel::MEDIUM, LODLevel::ULTRA, LODLevel::LOW };
    for (LODLevel fallbackLevel : fallbackOrder) {
        auto fallbackIt = m_lodMeshes.find(fallbackLevel);
        if (fallbackIt != m_lodMeshes.end()) {
            return fallbackIt->second;
        }
    }
    
    return nullptr;
}

std::shared_ptr<Mesh> MeshLOD::getMesh(LODLevel level) const {
    auto it = m_lodMeshes.find(level);
    return (it != m_lodMeshes.end()) ? it->second : nullptr;
}

bool MeshLOD::hasMesh(LODLevel level) const {
    return m_lodMeshes.find(level) != m_lodMeshes.end();
}

void MeshLOD::clear() {
    m_lodMeshes.clear();
}

LODLevel MeshLOD::calculateLOD(float distance, float objectRadius) {
    // Calculate a metric that combines distance and object size
    // Larger objects need higher detail even when far away
    // Smaller objects can use lower detail even when close
    float metric = distance / std::max(objectRadius, 0.1f); // Prevent division by zero
    
    // Select LOD based on metric (lower metric = closer/larger = higher detail)
    if (metric < s_highToUltraThreshold) {
        return LODLevel::ULTRA;
    } else if (metric < s_mediumToHighThreshold) {
        return LODLevel::HIGH;
    } else if (metric < s_lowToMediumThreshold) {
        return LODLevel::MEDIUM;
    } else {
        return LODLevel::LOW;
    }
}

void MeshLOD::setLODThresholds(float lowToMedium, float mediumToHigh, float highToUltra) {
    s_lowToMediumThreshold = lowToMedium;
    s_mediumToHighThreshold = mediumToHigh;
    s_highToUltraThreshold = highToUltra;
    
    std::cout << "MeshLOD: Updated thresholds - LOW→MEDIUM: " << lowToMedium 
              << ", MEDIUM→HIGH: " << mediumToHigh 
              << ", HIGH→ULTRA: " << highToUltra << std::endl;
}

