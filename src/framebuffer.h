#pragma once
#include <glad/glad.h>

class Framebuffer
{
  public:
    Framebuffer(unsigned int w, unsigned int h, bool withDepth = true);
    ~Framebuffer();

    Framebuffer(const Framebuffer&)            = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&)                 = default;
    Framebuffer& operator=(Framebuffer&&)      = default;

    void resize(unsigned int w, unsigned int h);
    void bind() const;
    void unbind() const;

    unsigned int colorTex() const
    {
        return _colorTex;
    }
    unsigned int getId() const
    {
        return _fbo;
    }
    unsigned int width() const
    {
        return _w;
    }
    unsigned int height() const
    {
        return _h;
    }

  private:
    void create();
    void destroy();

    GLuint       _fbo       = 0;
    GLuint       _colorTex  = 0;
    GLuint       _depthRb   = 0;
    unsigned int _w         = 0;
    unsigned int _h         = 0;
    bool         _withDepth = true;
};
