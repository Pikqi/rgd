#include "post_process_effect.h"
#include "primitives.h"
#include <glad/glad.h>
#include <rgd.h>

const Mesh& PostProcessEffect::sharedQuad()
{
    static const Mesh quad = primitives::createFullscreenQuad();
    return quad;
}

PostProcessEffect::PostProcessEffect(unsigned int w, unsigned int h,
                                     Shader* shader, const char* name)
    : _fbo(w, h, false), _shader(shader), _name(name), _w(w), _h(h)
{
    (void)sharedQuad(); // make sure we have a static shared quad
}

void PostProcessEffect::resize(unsigned int w, unsigned int h)
{
    _w = w;
    _h = h;
    _fbo.resize(w, h);
}

void PostProcessEffect::apply(unsigned int inputTex)
{
    GLCALL(
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo.colorTex() ? _fbo.getId() : 0));
    GLCALL(
        glViewport(0, 0, static_cast<GLsizei>(_w), static_cast<GLsizei>(_h)));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));

    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glDisable(GL_BLEND));

    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, inputTex));
    _shader->setInteger("uTex", 0);

    const Mesh& quad = sharedQuad();
    quad.bind();
    _shader->use();
    GLCALL(glDrawArrays(GL_TRIANGLES, 0, quad.getVertexCount()));

    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glEnable(GL_BLEND));
}
