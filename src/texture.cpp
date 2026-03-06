#include "texture.h"
#include "logger.h"
#include <rgd.h>
#include <glad/glad.h>

Texture::Texture(const unsigned char* data, unsigned int width,
                 unsigned int height, bool alpha)
    : _alpha(alpha), _width(width), _height(height)
{
    GLCALL(glGenTextures(1, &_id));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _id));

    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                        GL_UNSIGNED_BYTE, data));
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
void Texture::unbind() const
{
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0))
}
