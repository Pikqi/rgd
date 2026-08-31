#pragma once

#include "camera3d.h"
#include "framebuffer.h"
#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>

class Clouds
{
  public:
    Clouds(unsigned int viewportW, unsigned int viewportH);

    void resize(unsigned int viewportW, unsigned int viewportH);

    void draw(const Camera3D& camera, const glm::mat4& projection,
              const glm::vec3& sunDir, const glm::vec3& sunColor,
              const Framebuffer& sceneFbo, float time);

    float layerStart  = 25.0f;
    float layerHeight = 5.0f;
    float coverage    = 0.45f;
    float windSpeed   = 1.0f;
    int   stepCount   = 64;

  private:
    Shader&     _marchShader;
    Shader&     _compositeShader;
    Mesh        _quad;
    Framebuffer _fbo;

    unsigned int _viewW;
    unsigned int _viewH;
};
