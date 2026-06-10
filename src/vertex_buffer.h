#pragma once

class VertexBuffer
{

  public:
    VertexBuffer();
    VertexBuffer(const void* data, unsigned int size);
    void bind() const;
    void unbind() const;

  private:
    unsigned int _id;
};
