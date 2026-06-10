#pragma once
#include "shader.h"
#include <glad/glad.h>

class Texture
{
  public:
    Texture(const unsigned char* data, unsigned int width, unsigned int height,
            bool alpha, int num_of_channels);

    ~Texture();

    void setRepeat(bool set) const;
    void bind(unsigned int slot = 0) const;

    void bindOnShader(Shader& shader, unsigned int slot,
                      const char* uniform_name) const;
    void unbind() const;

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
