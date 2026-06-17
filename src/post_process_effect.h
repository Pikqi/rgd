#pragma once
#include "framebuffer.h"
#include "mesh.h"
#include "shader.h"

class PostProcessEffect
{
  public:
    PostProcessEffect(unsigned int w, unsigned int h, Shader* shader,
                      const char* name);
    ~PostProcessEffect() = default;

    PostProcessEffect(const PostProcessEffect&)            = delete;
    PostProcessEffect& operator=(const PostProcessEffect&) = delete;
    PostProcessEffect(PostProcessEffect&&)                 = default;
    PostProcessEffect& operator=(PostProcessEffect&&)      = default;

    void resize(unsigned int w, unsigned int h);
    void apply(unsigned int inputTex);

    unsigned int outputTex() const
    {
        return _fbo.colorTex();
    }
    const Framebuffer& fbo() const
    {
        return _fbo;
    }
    Shader* shader() const
    {
        return _shader;
    }
    const char* name() const
    {
        return _name;
    }

  private:
    static const Mesh& sharedQuad();

    Framebuffer  _fbo;
    Shader*      _shader;
    const char*  _name;
    unsigned int _w;
    unsigned int _h;
};
