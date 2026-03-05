#pragma once

class VertexBuffer
{

  public:
    VertexBuffer(const void* data, unsigned int size);
    void bind();
    void unbind();

  private:
    unsigned int _id;
};
