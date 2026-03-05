#include <glad/glad.h>
#include <vertex_buffer.h>

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
