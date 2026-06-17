#pragma once

#include "mesh.h"
#include <glm/glm.hpp>

struct NoiseParams
{
    float octaves = 4.0f;
    float frequency = 0.02f;
    float amplitude = 30.0f;
    float persistence = 0.5f;
    float lacunarity = 2.0f;
};

class TerrainGenerator
{
  public:
    static Mesh generate(const glm::ivec2& size, const NoiseParams& params);

  private:
    static float sampleNoise(float x, float z, const NoiseParams& params);
    static glm::vec3 calculateNormal(const std::vector<Vertex>& vertices,
                                     size_t index, const glm::ivec2& size);
};