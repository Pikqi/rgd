#pragma once

class IndexBuffer
{
  public:
    IndexBuffer(const void* data, unsigned int size);
    void bind() const;
    void unbind() const;

  private:
    unsigned int _id;
    unsigned int _size;
};
