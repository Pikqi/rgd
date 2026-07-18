#include "terrain_generator.h"
#include "logger.h"
#include <stb_perlin.h>

Mesh TerrainGenerator::generate(const glm::ivec2&  size,
                                const NoiseParams& params)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(size.x * size.y);

    for (int z = 0; z < size.y; ++z)
    {
        for (int x = 0; x < size.x; ++x)
        {
            Vertex vertex;
            vertex.position.x = static_cast<float>(x);
            vertex.position.z = static_cast<float>(z);
            vertex.position.y = sampleNoise(static_cast<float>(x),
                                            static_cast<float>(z), params);
            vertex.texCoord.x =
                static_cast<float>(x) / static_cast<float>(size.x);
            vertex.texCoord.y =
                static_cast<float>(z) / static_cast<float>(size.y);
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertices.push_back(vertex);
        }
    }

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].normal = calculateNormal(vertices, i, size);
    }

    for (int z = 0; z < size.y - 1; ++z)
    {
        for (int x = 0; x < size.x - 1; ++x)
        {
            unsigned int topLeft     = z * size.x + x;
            unsigned int topRight    = topLeft + 1;
            unsigned int bottomLeft  = (z + 1) * size.x + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setIndices(indices);
    mesh.uploadToGPU();

    LOG_INFO("Terrain generated: {} vertices, {} indices", vertices.size(),
             indices.size());

    return mesh;
}

float TerrainGenerator::sampleNoise(float x, float z, const NoiseParams& params)
{
    float noise = stb_perlin_fbm_noise3(
        x * params.frequency, 0.0f, z * params.frequency, params.lacunarity,
        params.persistence, static_cast<int>(params.octaves));

    return noise * params.amplitude;
}

glm::vec3 TerrainGenerator::calculateNormal(const std::vector<Vertex>& vertices,
                                            size_t                     index,
                                            const glm::ivec2&          size)
{
    const int x = static_cast<int>(index) % size.x;
    const int z = static_cast<int>(index) / size.x;

    const float hL =
        (x > 0) ? vertices[index - 1].position.y : vertices[index].position.y;
    const float hR = (x < size.x - 1) ? vertices[index + 1].position.y
                                      : vertices[index].position.y;
    const float hD = (z > 0) ? vertices[index - size.x].position.y
                             : vertices[index].position.y;
    const float hU = (z < size.y - 1) ? vertices[index + size.x].position.y
                                      : vertices[index].position.y;

    const glm::vec3 tx(2.0f, hR - hL, 0.0f);
    const glm::vec3 tz(0.0f, hU - hD, 2.0f);

    return glm::normalize(glm::cross(tz, tx));
}