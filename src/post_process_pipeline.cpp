#include "post_process_pipeline.h"
#include "logger.h"
#include "primitives.h"
#include "render_api.h"
#include <glad/glad.h>
#include <rgd.h>

PostProcessPipeline::PostProcessPipeline(unsigned int w, unsigned int h)
    : _scene(w, h, /*withDepth=*/true),
      _quad(primitives::createFullscreenQuad()), _w(w), _h(h)
{
}

void PostProcessPipeline::resize(unsigned int w, unsigned int h)
{
    _w = w;
    _h = h;
    _scene.resize(w, h);
}

void PostProcessPipeline::beginScene()
{
    _scene.bind();
    GLCALL(
        glViewport(0, 0, static_cast<GLsizei>(_w), static_cast<GLsizei>(_h)));
    RenderAPI::clear();
}

void PostProcessPipeline::present(Shader& finalShader)
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCALL(
        glViewport(0, 0, static_cast<GLsizei>(_w), static_cast<GLsizei>(_h)));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glDisable(GL_BLEND));

    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _scene.colorTex()));
    finalShader.setInteger("uTex", 0);

    _quad.bind();
    finalShader.use();
    GLCALL(glDrawArrays(GL_TRIANGLES, 0, _quad.getVertexCount()));

    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glEnable(GL_BLEND));
}
