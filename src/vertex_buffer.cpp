#include <glad/glad.h>
#include <vertex_buffer.h>
#include <rgd.h>

VertexBuffer::VertexBuffer() : _id(0) {}

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    GLCALL(glGenBuffers(1, &_id));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, _id));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}
void VertexBuffer::bind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, _id));
}

void VertexBuffer::unbind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
