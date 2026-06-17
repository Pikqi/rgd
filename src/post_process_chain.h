#pragma once
#include "framebuffer.h"
#include "post_process_effect.h"
#include "shader.h"
#include <memory>
#include <vector>

class PostProcessChain
{
  public:
    PostProcessChain(unsigned int w, unsigned int h);
    ~PostProcessChain() = default;

    PostProcessChain(const PostProcessChain&)            = delete;
    PostProcessChain& operator=(const PostProcessChain&) = delete;
    PostProcessChain(PostProcessChain&&)                 = default;
    PostProcessChain& operator=(PostProcessChain&&)      = default;

    void resize(unsigned int w, unsigned int h);

    void beginScene();
    void run(Shader& finalShader);

    PostProcessEffect& addEffect(Shader* shader, const char* name);

    const std::vector<std::unique_ptr<PostProcessEffect>>& effects() const
    {
        return _effects;
    }
    const Framebuffer& scene() const
    {
        return _scene;
    }

  private:
    Framebuffer                                     _scene;
    std::vector<std::unique_ptr<PostProcessEffect>> _effects;
    unsigned int                                    _w;
    unsigned int                                    _h;
};
