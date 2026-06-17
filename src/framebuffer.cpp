#include "framebuffer.h"
#include "logger.h"
#include <rgd.h>

Framebuffer::Framebuffer(unsigned int w, unsigned int h, bool withDepth)
    : _w(w), _h(h), _withDepth(withDepth)
{
    create();
}

Framebuffer::~Framebuffer()
{
    destroy();
}

void Framebuffer::create()
{
    GLCALL(glGenFramebuffers(1, &_fbo));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));

    GLCALL(glGenTextures(1, &_colorTex));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _colorTex));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(_w),
                        static_cast<GLsizei>(_h), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                        nullptr));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, _colorTex, 0));

    if (_withDepth)
    {
        GLCALL(glGenRenderbuffers(1, &_depthRb));
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, _depthRb));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                                     static_cast<GLsizei>(_w),
                                     static_cast<GLsizei>(_h)));
        GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                         GL_RENDERBUFFER, _depthRb));
    }

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_FATAL("Framebuffer incomplete: status=0x{:x}", status);
    }

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    if (_withDepth)
    {
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }
}

void Framebuffer::destroy()
{
    if (_fbo)
    {
        GLCALL(glDeleteFramebuffers(1, &_fbo));
        _fbo = 0;
    }
    if (_colorTex)
    {
        GLCALL(glDeleteTextures(1, &_colorTex));
        _colorTex = 0;
    }
    if (_depthRb)
    {
        GLCALL(glDeleteRenderbuffers(1, &_depthRb));
        _depthRb = 0;
    }
}

void Framebuffer::resize(unsigned int w, unsigned int h)
{
    if (w == _w && h == _h && _fbo != 0)
    {
        return;
    }
    destroy();
    _w = w;
    _h = h;
    create();
}

void Framebuffer::bind() const
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
}

void Framebuffer::unbind() const
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
