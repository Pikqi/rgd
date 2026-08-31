#pragma once

#include "camera3d.h"
#include "framebuffer.h"
#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>

class Water
{
  public:
    Water(unsigned int viewportW, unsigned int viewportH);

    void resize(unsigned int viewportW, unsigned int viewportH);

    // Render water into sceneFbo, sampling a snapshot of sceneFbo (taken at the
    // start of this call) for refraction and depth-tint.
    void draw(const Camera3D& camera, const glm::mat4& projection,
              const glm::vec3& sunDir, const glm::vec3& sunColor,
              const Framebuffer& sceneFbo, float time);

    // Center of the water plane in world space (XZ); Y comes from seaLevel.
    glm::vec3 planeCenter        = glm::vec3(64.0f, 0.0f, 64.0f);
    float     seaLevel           = 0.0f;
    float     waveAmplitude      = 0.7f;
    float     shininess          = 250.0f;
    glm::vec3 waterColor         = glm::vec3(0.04f, 0.18f, 0.30f);
    glm::vec3 horizonColor       = glm::vec3(0.70f, 0.80f, 0.90f);
    float     distortionStrength = 0.03f; // refraction UV wobble
    float     depthDarkness      = 0.8f; // deep-water darkening + bottom hiding
    float     maxWaterDepth      = 12.0f; // depth at full darkness

  private:
    Shader&      _shader;
    Mesh         _mesh;
    Framebuffer  _sceneSnapshot; // copy of scene color+depth before water draw
    unsigned int _viewW;
    unsigned int _viewH;
};
