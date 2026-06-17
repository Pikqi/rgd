#pragma once

#include "index_buffer.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

class Mesh
{
  public:
    Mesh();
    ~Mesh();

    Mesh(Mesh&&)                 = default;
    Mesh& operator=(Mesh&&)      = default;
    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    void setVertices(const std::vector<Vertex>& vertices);
    void setIndices(const std::vector<unsigned int>& indices);
    void uploadToGPU();

    void bind() const;

    unsigned int getIndexCount() const
    {
        return static_cast<unsigned int>(_indexCount);
    }
    unsigned int getVertexCount() const
    {
        return static_cast<unsigned int>(_vertexCount);
    }
    bool isUploaded() const
    {
        return _uploaded;
    }

  private:
    std::unique_ptr<VertexArray>  _vao;
    std::unique_ptr<VertexBuffer> _vbo;
    std::unique_ptr<IndexBuffer>  _ibo;
    size_t                        _vertexCount = 0;
    size_t                        _indexCount  = 0;
    bool                          _uploaded    = false;
};
