#include "clouds.h"

#include "primitives.h"
#include "resource_manager.h"
#include "rgd.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_inverse.hpp>

Clouds::Clouds(unsigned int viewportW, unsigned int viewportH)
    : _fbo(std::max(1u, viewportW / 2), std::max(1u, viewportH / 2), false),
      _viewW(viewportW), _viewH(viewportH),

      _marchShader(ResourceManager::LoadShader("shaders/clouds/vertex.glsl",
                                               "shaders/clouds/fragment.glsl",
                                               "clouds_march")),

      _compositeShader(ResourceManager::LoadShader(
          "shaders/clouds/vertex.glsl", "shaders/clouds/composite.frag",
          "clouds_composite")

      )

{
    _quad = primitives::createFullscreenQuad();
}

void Clouds::resize(unsigned int viewportW, unsigned int viewportH)
{
    _viewW = viewportW;
    _viewH = viewportH;
    _fbo.resize(std::max(1u, viewportW / 2), std::max(1u, viewportH / 2));
}

void Clouds::draw(const Camera3D& camera, const glm::mat4& projection,
                  const glm::vec3& sunDir, const glm::vec3& sunColor,
                  const Framebuffer& sceneFbo, float time)
{
    glm::mat4 invVP = glm::inverse(projection * camera.getViewMatrix());

    const unsigned int halfW = std::max(1u, _viewW / 2);
    const unsigned int halfH = std::max(1u, _viewH / 2);

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo.getId()));
    GLCALL(glViewport(0, 0, static_cast<GLsizei>(halfW),
                      static_cast<GLsizei>(halfH)));
    GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));

    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glDisable(GL_BLEND));

    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, sceneFbo.depthTex()));

    _marchShader.use();
    _marchShader.setInteger("uSceneDepth", 0);
    _marchShader.setMatrix4("uInvViewProj", invVP);
    _marchShader.setVector3f("uCameraPos", camera.position);
    _marchShader.setVector3f("uSunDir", sunDir);
    _marchShader.setVector3f("uSunColor", sunColor);
    _marchShader.setFloat("uLayerStart", layerStart);
    _marchShader.setFloat("uLayerEnd", layerStart + layerHeight);
    _marchShader.setFloat("uCoverage", coverage);
    _marchShader.setInteger("uStepCount", stepCount);
    _marchShader.setFloat("uTime", time * 0.1f * windSpeed);
    _marchShader.setInteger("uOffsetStart", enableOffsetStart);

    _quad.bind();
    GLCALL(glDrawArrays(GL_TRIANGLES, 0, _quad.getVertexCount()));

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo.getId()));
    GLCALL(glViewport(0, 0, static_cast<GLsizei>(sceneFbo.width()),
                      static_cast<GLsizei>(sceneFbo.height())));

    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));

    GLCALL(glActiveTexture(GL_TEXTURE0));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _fbo.colorTex()));
    _compositeShader.use();
    _compositeShader.setInteger("uCloudsTex", 0);

    _quad.bind();
    GLCALL(glDrawArrays(GL_TRIANGLES, 0, _quad.getVertexCount()));

    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

    // Restore default blend state expected by the rest of the pipeline.
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glEnable(GL_BLEND));
}
