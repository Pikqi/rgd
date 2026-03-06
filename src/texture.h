#pragma once
#include <glad/glad.h>

class Texture
{
  public:
    Texture(const unsigned char* data, unsigned int width, unsigned int height,
            bool alpha);

    ~Texture();
    void bind(unsigned int slot = 0);
    void unbind();

    inline unsigned int getWidth()
    {
        return _width;
    }

    inline unsigned int getHeight()
    {
        return _height;
    }

    inline unsigned int getId()
    {
        return _id;
    }

  private:
    unsigned int _id;
    unsigned int _width;
    unsigned int _height;
    bool         _alpha;
};
