#pragma once

#include "camera3d.h"
#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>

class Sky
{
  public:
    Sky();

    void draw(const Camera3D& camera, const glm::mat4& projection,
              const glm::vec3& sunDir, const glm::vec3& sunColor);

    glm::vec3 zenithColor  = glm::vec3(0.10f, 0.25f, 0.55f);
    glm::vec3 horizonColor = glm::vec3(0.70f, 0.80f, 0.90f);

  private:
    Shader& _shader;
    Mesh    _quad;
};
