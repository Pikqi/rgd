#include "post_processor.h"
#include <vertex_array.h>

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &_id);
    glBindVertexArray(_id);
};

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &_id);
};

void VertexArray::bind() const
{
    glBindVertexArray(_id);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::add_buffer(const VertexBuffer&       vb,
                             const VertexBufferLayout& layout)
{
    vb.bind();
    const auto& elements = layout.get_elements();

    unsigned int offset = 0;
    for (int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];

        const auto element_size =
            VertexBufferElement::get_size_of_type(element.type) * element.count;

        glVertexAttribPointer(i, element.count, element.type,
                              element.normalize ? GL_TRUE : GL_FALSE,
                              layout.get_stride(), (void*)offset);
        offset += element_size;
        glEnableVertexAttribArray(i);
    }
}
std::vector<VertexBufferElement> VertexBufferLayout::get_elements() const
{
    return _elements;
}

unsigned int VertexBufferLayout::get_stride() const
{
    return _stride;
}
