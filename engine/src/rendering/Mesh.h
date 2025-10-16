#pragma once

#include <vector>
#include <glm/glm.hpp>

// Forward declare OpenGL types
typedef unsigned int GLuint;

// Manages OpenGL mesh data (VAO, VBO, EBO)
class Mesh {
public:
    Mesh();
    ~Mesh();
    
    // Load mesh data (legacy - positions only)
    void loadVertices(const std::vector<float>& vertices);
    void loadVertices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    
    // Load mesh data with custom vertex layout
    void loadVerticesWithLayout(const std::vector<float>& vertices, bool hasNormals, bool hasUVs);
    void loadVerticesWithLayout(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, bool hasNormals, bool hasUVs);
    
    // Render the mesh
    void draw() const;
    
    // Get vertex count
    size_t getVertexCount() const { return m_vertexCount; }
    size_t getIndexCount() const { return m_indexCount; }
    
    // Get mesh properties
    bool hasNormals() const { return m_hasNormals; }
    bool hasUVs() const { return m_hasUVs; }

private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    
    size_t m_vertexCount;
    size_t m_indexCount;
    bool m_hasIndices;
    bool m_hasNormals;
    bool m_hasUVs;
    
    void cleanup();
    void setupVertexAttributes(bool hasNormals, bool hasUVs);
};
