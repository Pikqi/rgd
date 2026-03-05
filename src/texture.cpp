#include "texture.h"
#include <rgd.h>

Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB),
      Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR),
      Filter_Max(GL_NEAREST)
{
    GLCALL(glGenTextures(1, &this->ID));
}

void Texture2D::Generate(unsigned int width, unsigned int height,
                         unsigned char* data)
{
    this->Width  = width;
    this->Height = height;
    // create Texture
    GLCALL(glBindTexture(GL_TEXTURE_2D, this->ID));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height,
                        0, this->Image_Format, GL_UNSIGNED_BYTE, data));
    // set Texture wrap and filter modes
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                           this->Filter_Min));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                           this->Filter_Max));
    // unbind texture
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::Bind() const
{
    GLCALL(glBindTexture(GL_TEXTURE_2D, this->ID));
}
