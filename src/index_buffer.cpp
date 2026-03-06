#include <index_buffer.h>
#include <glad/glad.h>
#include "rgd.h"

IndexBuffer::IndexBuffer(const void* data, unsigned int count)
    : _count(count * sizeof(unsigned int))
{
    GLCALL(glGenBuffers(1, &_id));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int),
                        data, GL_STATIC_DRAW));
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void IndexBuffer::bind() const
{
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id));
}
void IndexBuffer::unbind() const
{
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
