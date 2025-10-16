#include "MeshLoader.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <algorithm>

std::shared_ptr<Mesh> MeshLoader::load(const std::string& filepath) {
    return loadMeshFromAssimp(filepath, false, false);
}

std::shared_ptr<Mesh> MeshLoader::loadWithNormals(const std::string& filepath) {
    return loadMeshFromAssimp(filepath, true, false);
}

std::shared_ptr<Mesh> MeshLoader::loadWithNormalsAndUVs(const std::string& filepath) {
    return loadMeshFromAssimp(filepath, true, true);
}

std::vector<std::shared_ptr<Mesh>> MeshLoader::loadAll(const std::string& filepath) {
    std::vector<std::shared_ptr<Mesh>> meshes;
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices
    );
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "MeshLoader ERROR: Failed to load file: " << filepath << std::endl;
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return meshes;
    }
    
    extractMeshData(scene, meshes, true, true);
    
    std::cout << "MeshLoader: Successfully loaded " << meshes.size() << " mesh(es) from " << filepath << std::endl;
    return meshes;
}

MeshLoader::MeshInfo MeshLoader::getMeshInfo(const std::string& filepath) {
    MeshInfo info;
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "MeshLoader ERROR: Failed to load file info: " << filepath << std::endl;
        return info;
    }
    
    info.meshCount = scene->mNumMeshes;
    
    // Aggregate stats from all meshes
    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);
    
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        info.vertexCount += mesh->mNumVertices;
        info.triangleCount += mesh->mNumFaces;
        info.hasNormals = info.hasNormals || mesh->HasNormals();
        info.hasUVs = info.hasUVs || mesh->HasTextureCoords(0);
        info.hasColors = info.hasColors || mesh->HasVertexColors(0);
        
        // Calculate bounding box
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            minBound.x = std::min(minBound.x, vertex.x);
            minBound.y = std::min(minBound.y, vertex.y);
            minBound.z = std::min(minBound.z, vertex.z);
            maxBound.x = std::max(maxBound.x, vertex.x);
            maxBound.y = std::max(maxBound.y, vertex.y);
            maxBound.z = std::max(maxBound.z, vertex.z);
        }
    }
    
    info.boundingBoxMin = minBound;
    info.boundingBoxMax = maxBound;
    
    return info;
}

bool MeshLoader::isFormatSupported(const std::string& filepath) {
    // Extract extension
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) return false;
    
    std::string ext = filepath.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // Assimp supports these formats (most common ones)
    const std::vector<std::string> supportedFormats = {
        "obj", "fbx", "gltf", "glb", "dae", "blend", "3ds", "ase",
        "ply", "stl", "dxf", "ifc", "nff", "smd", "vta", "mdl",
        "md2", "md3", "mdc", "md5mesh", "x", "q3o", "q3s", "ac",
        "ms3d", "cob", "scn", "bvh", "csm", "xml", "irrmesh", "irr",
        "off", "ter", "hmp", "mesh.xml", "skeleton.xml", "material",
        "ogre", "opengex", "3d", "b3d", "dae", "lwo", "lws", "lxo"
    };
    
    return std::find(supportedFormats.begin(), supportedFormats.end(), ext) != supportedFormats.end();
}

std::vector<std::string> MeshLoader::getSupportedFormats() {
    return {
        "obj", "fbx", "gltf", "glb", "dae", "blend", "3ds", "ase",
        "ply", "stl", "dxf", "ifc", "nff", "smd", "vta", "mdl",
        "md2", "md3", "mdc", "md5mesh", "x", "q3o", "q3s", "ac",
        "ms3d", "cob", "scn", "bvh", "csm", "xml", "irrmesh", "irr",
        "off", "ter", "hmp", "mesh.xml", "skeleton.xml", "material",
        "ogre", "opengex", "3d", "b3d", "lwo", "lws", "lxo"
    };
}

std::shared_ptr<Mesh> MeshLoader::loadMeshFromAssimp(const std::string& filepath, bool includeNormals, bool includeUVs) {
    Assimp::Importer importer;
    
    // Configure post-processing flags
    unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
    if (includeNormals) {
        flags |= aiProcess_GenNormals;
    }
    if (includeUVs) {
        flags |= aiProcess_FlipUVs;
    }
    
    const aiScene* scene = importer.ReadFile(filepath, flags);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "MeshLoader ERROR: Failed to load file: " << filepath << std::endl;
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }
    
    // For single mesh loading, just take the first mesh
    if (scene->mNumMeshes == 0) {
        std::cerr << "MeshLoader ERROR: No meshes found in file: " << filepath << std::endl;
        return nullptr;
    }
    
    aiMesh* assimpMesh = scene->mMeshes[0];
    auto mesh = std::make_shared<Mesh>();
    
    // Extract vertex data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Vertex layout: position [normal] [uv]
    unsigned int stride = 3; // position (x, y, z)
    if (includeNormals) stride += 3; // + normal (nx, ny, nz)
    if (includeUVs) stride += 2; // + uv (u, v)
    
    for (unsigned int i = 0; i < assimpMesh->mNumVertices; i++) {
        // Position
        vertices.push_back(assimpMesh->mVertices[i].x);
        vertices.push_back(assimpMesh->mVertices[i].y);
        vertices.push_back(assimpMesh->mVertices[i].z);
        
        // Normals
        if (includeNormals && assimpMesh->HasNormals()) {
            vertices.push_back(assimpMesh->mNormals[i].x);
            vertices.push_back(assimpMesh->mNormals[i].y);
            vertices.push_back(assimpMesh->mNormals[i].z);
        } else if (includeNormals) {
            // Default normals if not present
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
        }
        
        // UVs
        if (includeUVs && assimpMesh->HasTextureCoords(0)) {
            vertices.push_back(assimpMesh->mTextureCoords[0][i].x);
            vertices.push_back(assimpMesh->mTextureCoords[0][i].y);
        } else if (includeUVs) {
            // Default UVs if not present
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }
    
    // Extract indices
    for (unsigned int i = 0; i < assimpMesh->mNumFaces; i++) {
        aiFace face = assimpMesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    // Load into Mesh object
    if (!indices.empty()) {
        mesh->loadVerticesWithLayout(vertices, indices, includeNormals, includeUVs);
    } else {
        mesh->loadVerticesWithLayout(vertices, includeNormals, includeUVs);
    }
    
    std::cout << "MeshLoader: Successfully loaded mesh from " << filepath 
              << " (" << assimpMesh->mNumVertices << " vertices, " 
              << assimpMesh->mNumFaces << " faces)" << std::endl;
    
    return mesh;
}

void MeshLoader::extractMeshData(const void* scenePtr, std::vector<std::shared_ptr<Mesh>>& meshes, bool includeNormals, bool includeUVs) {
    const aiScene* scene = static_cast<const aiScene*>(scenePtr);
    
    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* assimpMesh = scene->mMeshes[m];
        auto mesh = std::make_shared<Mesh>();
        
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        
        // Vertex layout
        unsigned int stride = 3;
        if (includeNormals) stride += 3;
        if (includeUVs) stride += 2;
        
        for (unsigned int i = 0; i < assimpMesh->mNumVertices; i++) {
            vertices.push_back(assimpMesh->mVertices[i].x);
            vertices.push_back(assimpMesh->mVertices[i].y);
            vertices.push_back(assimpMesh->mVertices[i].z);
            
            if (includeNormals && assimpMesh->HasNormals()) {
                vertices.push_back(assimpMesh->mNormals[i].x);
                vertices.push_back(assimpMesh->mNormals[i].y);
                vertices.push_back(assimpMesh->mNormals[i].z);
            } else if (includeNormals) {
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }
            
            if (includeUVs && assimpMesh->HasTextureCoords(0)) {
                vertices.push_back(assimpMesh->mTextureCoords[0][i].x);
                vertices.push_back(assimpMesh->mTextureCoords[0][i].y);
            } else if (includeUVs) {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }
        
        for (unsigned int i = 0; i < assimpMesh->mNumFaces; i++) {
            aiFace face = assimpMesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
        
        if (!indices.empty()) {
            mesh->loadVerticesWithLayout(vertices, indices, includeNormals, includeUVs);
        } else {
            mesh->loadVerticesWithLayout(vertices, includeNormals, includeUVs);
        }
        
        meshes.push_back(mesh);
    }
}

