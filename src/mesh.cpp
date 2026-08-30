#include "mesh.h"
#include "logger.h"
#include <rgd.h>

Mesh::Mesh() {}

Mesh::~Mesh() {}

void Mesh::setVertices(const std::vector<Vertex>& vertices)
{
    _vertexCount = vertices.size();
    _vbo = std::make_unique<VertexBuffer>(vertices.data(),
                                          vertices.size() * sizeof(Vertex));
}

void Mesh::setIndices(const std::vector<unsigned int>& indices)
{
    _indexCount = indices.size();
    GLCALL(glBindVertexArray(0));
    _ibo = std::make_unique<IndexBuffer>(indices.data(), indices.size());
}

void Mesh::uploadToGPU()
{
    if (_vertexCount == 0 || _indexCount == 0)
    {
        LOG_WARN("Mesh: No vertices or indices to upload");
        return;
    }

    _vao = std::make_unique<VertexArray>();
    _vao->bind();
    _vbo->bind();
    _ibo->bind();

    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    layout.push(GL_FLOAT, 2);
    layout.push(GL_FLOAT, 3);
    _vao->add_buffer(*_vbo, layout);

    _uploaded = true;

    _vao->unbind();
}

void Mesh::bind() const
{
    if (_vao)
        _vao->bind();
}
