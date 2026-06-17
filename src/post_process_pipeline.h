#pragma once
#include "framebuffer.h"
#include "mesh.h"
#include "shader.h"

class PostProcessPipeline
{
  public:
    PostProcessPipeline(unsigned int w, unsigned int h);
    ~PostProcessPipeline() = default;

    PostProcessPipeline(const PostProcessPipeline&)            = delete;
    PostProcessPipeline& operator=(const PostProcessPipeline&) = delete;
    PostProcessPipeline(PostProcessPipeline&&)                 = default;
    PostProcessPipeline& operator=(PostProcessPipeline&&)      = default;

    void resize(unsigned int w, unsigned int h);

    void beginScene();
    void present(Shader& finalShader);

    unsigned int sceneColorTex() const
    {
        return _scene.colorTex();
    }

  private:
    Framebuffer  _scene;
    Mesh         _quad;
    unsigned int _w = 0;
    unsigned int _h = 0;
};
