#pragma once

class IndexBuffer
{
  public:
    IndexBuffer();
    IndexBuffer(const void* data, unsigned int count);
    void bind() const;
    void unbind() const;

    inline unsigned int get_count() const
    {
        return _count;
    }

  private:
    unsigned int _id;
    unsigned int _count;
};
