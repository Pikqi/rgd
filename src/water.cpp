#include "water.h"

#include "primitives.h"
#include "resource_manager.h"
#include "rgd.h"

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

Water::Water(unsigned int viewportW, unsigned int viewportH)
    : _sceneSnapshot(viewportW, viewportH, true), _viewW(viewportW),
      _viewH(viewportH)
{
    _shader = ResourceManager::LoadShader("shaders/water/vertex.glsl",
                                          "shaders/water/fragment.glsl",
                                          nullptr, "water");
    _mesh   = primitives::createPlaneXZ(1000.0f);
}

void Water::resize(unsigned int viewportW, unsigned int viewportH)
{
    _viewW = viewportW;
    _viewH = viewportH;
    _sceneSnapshot.resize(viewportW, viewportH);
}

void Water::draw(const Camera3D& camera, const glm::mat4& projection,
                 const glm::vec3& sunDir, const glm::vec3& sunColor,
                 const Framebuffer& sceneFbo, float time)
{
    // Sampling sceneFbo's own attachments while rendering into it is undefined
    // behavior, so we blit into _sceneSnapshot first.
    GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFbo.getId()));
    GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _sceneSnapshot.getId()));
    GLCALL(glBlitFramebuffer(
        0, 0, static_cast<GLsizei>(_viewW), static_cast<GLsizei>(_viewH), 0, 0,
        static_cast<GLsizei>(_viewW), static_cast<GLsizei>(_viewH),
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST));

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo.getId()));
    GLCALL(glViewport(0, 0, static_cast<GLsizei>(_viewW),
                      static_cast<GLsizei>(_viewH)));

    glm::mat4 model = glm::translate(
        glm::mat4(1.0f), glm::vec3(planeCenter.x, seaLevel, planeCenter.z));
    glm::mat4 invViewProj = glm::inverse(projection * camera.getViewMatrix());

    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glDepthMask(GL_TRUE));
    GLCALL(glDisable(GL_BLEND));

    GLCALL(glActiveTexture(GL_TEXTURE1));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _sceneSnapshot.colorTex()));
    GLCALL(glActiveTexture(GL_TEXTURE2));
    GLCALL(glBindTexture(GL_TEXTURE_2D, _sceneSnapshot.depthTex()));

    _shader.use();
    _shader.setMatrix4("model", model);
    _shader.setMatrix4("view", camera.getViewMatrix());
    _shader.setMatrix4("projection", projection);
    _shader.setMatrix4("uInvViewProj", invViewProj);
    _shader.setVector3f("uSunDir", sunDir);
    _shader.setVector3f("uSunColor", sunColor);
    _shader.setVector3f("uCameraPos", camera.position);
    _shader.setFloat("uTime", time);
    _shader.setFloat("uWaveAmplitude", waveAmplitude);
    _shader.setFloat("uShininess", shininess);
    _shader.setVector3f("uWaterColor", waterColor);
    _shader.setVector3f("uHorizonColor", horizonColor);
    _shader.setFloat("uSeaLevel", seaLevel);
    _shader.setFloat("uDistortionStrength", distortionStrength);
    _shader.setFloat("uDepthDarkness", depthDarkness);
    _shader.setFloat("uMaxWaterDepth", maxWaterDepth);
    _shader.setInteger("uSceneColorTex", 1);
    _shader.setInteger("uSceneDepthTex", 2);
    _mesh.bind();
    GLCALL(glDrawElements(GL_TRIANGLES,
                          static_cast<GLsizei>(_mesh.getIndexCount()),
                          GL_UNSIGNED_INT, nullptr));

    GLCALL(glActiveTexture(GL_TEXTURE1));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    GLCALL(glActiveTexture(GL_TEXTURE2));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    GLCALL(glActiveTexture(GL_TEXTURE0));
}
