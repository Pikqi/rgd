#pragma once
#include "logger.h"
#include "vertex_buffer.h"
#include <vector>
#include <glad/glad.h>

struct VertexBufferElement
{
    unsigned int        type;
    unsigned int        count;
    bool                normalize;
    static unsigned int get_size_of_type(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        }
        LOG_FATAL("Unkown VertexBufferElement type, cant tell size.");
        return 0;
    }
};

class VertexBufferLayout
{
  public:
    void push(unsigned int type, unsigned int count)
    {
        _stride += count * VertexBufferElement::get_size_of_type(type);
        _elements.push_back({type, count, false});
    }

    std::vector<VertexBufferElement> get_elements() const;
    unsigned int                     get_stride() const;

  private:
    std::vector<VertexBufferElement> _elements;
    unsigned int                     _stride = 0;
};

class VertexArray
{
  public:
    VertexArray();
    VertexArray(const VertexArray&) = delete;
    VertexArray(VertexArray&&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray& operator=(VertexArray&&) = delete;
    ~VertexArray();

    void add_buffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

    void bind() const;
    void unbind() const;

  private:
    unsigned int _id = 0;
};
