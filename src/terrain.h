#pragma once

#include "mesh.h"
#include "terrain_generator.h"
#include <glm/glm.hpp>

class Terrain
{
  public:
    Terrain(const glm::ivec2& size, const NoiseParams& params);

    void regenerate(const NoiseParams& params);
    void draw(const Shader& shader) const;

    const glm::mat4& getTransform() const
    {
        return _transform;
    }
    void setTransform(const glm::mat4& transform)
    {
        _transform = transform;
    }

  private:
    Mesh        _mesh;
    glm::ivec2  _size;
    NoiseParams _params;
    glm::mat4   _transform = glm::mat4(1.0f);
};