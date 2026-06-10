#include "texture.h"
#include "logger.h"
#include "shader.h"
#include <rgd.h>
#include <glad/glad.h>

Texture::Texture(const unsigned char* data, unsigned int width,
                 unsigned int height, bool alpha, int num_of_channels)
    : _alpha(alpha), _width(width), _height(height)
{
    GLCALL(glGenTextures(1, &_id));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _id));

    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    switch (num_of_channels)
    {
    case 1:
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                            GL_UNSIGNED_BYTE, data));
        break;
    case 3:
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, data));
        break;
    case 4:
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                            GL_RGBA, GL_UNSIGNED_BYTE, data));
        break;
    default:
        LOG_FATAL("Cant create texture for number of channels: {}",
                  num_of_channels);
    }
}
Texture::~Texture()
{
    GLCALL(glDeleteTextures(1, &_id));
}

void Texture::setRepeat(bool set) const
{
    if (set)
    {
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    }
    else
    {
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                               GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                               GL_CLAMP_TO_EDGE));
    }
}

void Texture::bind(unsigned int slot) const
{
    if (slot > 31)
    {
        LOG_WARN("Tried to bind texture to slot > 31, slot: {}", slot);
        return;
    }

    GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _id));
};

void Texture::bindOnShader(Shader& shader, unsigned int slot,
                           const char* uniform_name) const
{
    bind(slot);
    shader.setInteger(uniform_name, slot);
}

void Texture::unbind() const
{
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}
