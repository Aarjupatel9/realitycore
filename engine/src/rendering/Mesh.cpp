#include "Mesh.h"
#include <glad/glad.h>
#include <iostream>

Mesh::Mesh() : m_VAO(0), m_VBO(0), m_EBO(0), m_vertexCount(0), m_indexCount(0), m_hasIndices(false), m_hasNormals(false), m_hasUVs(false) {}

Mesh::~Mesh() {
    cleanup();
}

void Mesh::loadVertices(const std::vector<float>& vertices) {
    cleanup();
    
    m_vertexCount = vertices.size() / 3; // Assuming 3 components per vertex
    m_hasIndices = false;
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Mesh::loadVertices(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    cleanup();
    
    m_vertexCount = vertices.size() / 3; // Assuming 3 components per vertex
    m_indexCount = indices.size();
    m_hasIndices = true;
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(m_VAO);
    if (m_hasIndices) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
    }
    glBindVertexArray(0);
}

void Mesh::loadVerticesWithLayout(const std::vector<float>& vertices, bool hasNormals, bool hasUVs) {
    cleanup();
    
    m_hasNormals = hasNormals;
    m_hasUVs = hasUVs;
    m_hasIndices = false;
    
    // Calculate stride: position(3) + normal(3 if present) + uv(2 if present)
    unsigned int stride = 3;
    if (hasNormals) stride += 3;
    if (hasUVs) stride += 2;
    
    m_vertexCount = vertices.size() / stride;
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    setupVertexAttributes(hasNormals, hasUVs);
    
    glBindVertexArray(0);
}

void Mesh::loadVerticesWithLayout(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, bool hasNormals, bool hasUVs) {
    cleanup();
    
    m_hasNormals = hasNormals;
    m_hasUVs = hasUVs;
    m_hasIndices = true;
    m_indexCount = indices.size();
    
    // Calculate stride
    unsigned int stride = 3;
    if (hasNormals) stride += 3;
    if (hasUVs) stride += 2;
    
    m_vertexCount = vertices.size() / stride;
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    setupVertexAttributes(hasNormals, hasUVs);
    
    glBindVertexArray(0);
}

void Mesh::setupVertexAttributes(bool hasNormals, bool hasUVs) {
    // Calculate stride
    unsigned int stride = 3;
    if (hasNormals) stride += 3;
    if (hasUVs) stride += 2;
    
    unsigned int strideBytes = stride * sizeof(float);
    unsigned int offset = 0;
    
    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideBytes, (void*)(offset * sizeof(float)));
    glEnableVertexAttribArray(0);
    offset += 3;
    
    // Normal attribute (location 1)
    if (hasNormals) {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideBytes, (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(1);
        offset += 3;
    }
    
    // UV attribute (location 2)
    if (hasUVs) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideBytes, (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(2);
        offset += 2;
    }
}

void Mesh::cleanup() {
    if (m_EBO) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    m_vertexCount = 0;
    m_indexCount = 0;
    m_hasIndices = false;
    m_hasNormals = false;
    m_hasUVs = false;
}
