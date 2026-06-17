#include "terrain.h"

Terrain::Terrain(const glm::ivec2& size, const NoiseParams& params)
    : _size(size), _params(params)
{
    _mesh = TerrainGenerator::generate(size, params);
}

void Terrain::regenerate(const NoiseParams& params)
{
    _params = params;
    _mesh = TerrainGenerator::generate(_size, params);
}

void Terrain::draw(const Shader& shader) const
{
    _mesh.draw(shader);
}