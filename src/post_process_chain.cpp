#include "post_process_chain.h"
#include "post_process_effect.h"
#include "primitives.h"
#include "render_api.h"
#include <glad/glad.h>
#include <rgd.h>

namespace {
const Mesh& sharedQuad()
{
    static const Mesh quad = primitives::createFullscreenQuad();
    return quad;
}
} // namespace

PostProcessChain::PostProcessChain(unsigned int w, unsigned int h)
    : _scene(w, h, true), _w(w), _h(h)
{
    (void)sharedQuad();
}

void PostProcessChain::resize(unsigned int w, unsigned int h)
{
    _w = w;
    _h = h;
    _scene.resize(w, h);
    for (auto& e : _effects)
    {
        e->resize(w, h);
    }
}

void PostProcessChain::beginScene()
{
    _scene.bind();
    GLCALL(
        glViewport(0, 0, static_cast<GLsizei>(_w), static_cast<GLsizei>(_h)));
    RenderAPI::clear();
}

PostProcessEffect& PostProcessChain::addEffect(Shader* shader, const char* name)
{
    _effects.push_back(
        std::make_unique<PostProcessEffect>(_w, _h, shader, name));
    return *_effects.back();
}

void PostProcessChain::run(Shader& finalShader)
{
    unsigned int currentTex = _scene.colorTex();

    for (auto& e : _effects)
    {
        e->apply(currentTex);
        currentTex = e->outputTex();
    }

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCALL(
        glViewport(0, 0, static_cast<GLsizei>(_w), static_cast<GLsizei>(_h)));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glDisable(GL_BLEND));

    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, currentTex));
    finalShader.setInteger("uTex", 0);

    const Mesh& quad = sharedQuad();
    quad.bind();
    finalShader.use();
    GLCALL(glDrawArrays(GL_TRIANGLES, 0, quad.getVertexCount()));

    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glEnable(GL_BLEND));
}
