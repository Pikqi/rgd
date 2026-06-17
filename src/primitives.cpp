#include "primitives.h"
#include <glm/glm.hpp>
#include <vector>

namespace primitives {

namespace {

void buildFace(std::vector<Vertex>&       vertices,
               std::vector<unsigned int>& indices, const glm::vec3& a,
               const glm::vec3& b, const glm::vec3& c, const glm::vec3& d,
               const glm::vec3& normal)
{
    unsigned int base = static_cast<unsigned int>(vertices.size());

    glm::vec2 uv_bl = glm::vec2(0.0f, 0.0f);
    glm::vec2 uv_br = glm::vec2(1.0f, 0.0f);
    glm::vec2 uv_tr = glm::vec2(1.0f, 1.0f);
    glm::vec2 uv_tl = glm::vec2(0.0f, 1.0f);

    vertices.push_back({a, uv_bl, normal});
    vertices.push_back({b, uv_br, normal});
    vertices.push_back({c, uv_tr, normal});
    vertices.push_back({d, uv_tl, normal});

    indices.push_back(base + 0);
    indices.push_back(base + 1);
    indices.push_back(base + 2);

    indices.push_back(base + 0);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
}

} // namespace

Mesh createCube()
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(24);
    indices.reserve(36);

    const float s = 0.5f;

    // +X (right)
    buildFace(vertices, indices, glm::vec3(s, -s, -s), glm::vec3(s, -s, s),
              glm::vec3(s, s, s), glm::vec3(s, s, -s),
              glm::vec3(1.0f, 0.0f, 0.0f));
    // -X (left)
    buildFace(vertices, indices, glm::vec3(-s, -s, s), glm::vec3(-s, -s, -s),
              glm::vec3(-s, s, -s), glm::vec3(-s, s, s),
              glm::vec3(-1.0f, 0.0f, 0.0f));
    // +Y (top)
    buildFace(vertices, indices, glm::vec3(-s, s, -s), glm::vec3(s, s, -s),
              glm::vec3(s, s, s), glm::vec3(-s, s, s),
              glm::vec3(0.0f, 1.0f, 0.0f));
    // -Y (bottom)
    buildFace(vertices, indices, glm::vec3(-s, -s, s), glm::vec3(s, -s, s),
              glm::vec3(s, -s, -s), glm::vec3(-s, -s, -s),
              glm::vec3(0.0f, -1.0f, 0.0f));
    // +Z (front)
    buildFace(vertices, indices, glm::vec3(-s, -s, s), glm::vec3(s, -s, s),
              glm::vec3(s, s, s), glm::vec3(-s, s, s),
              glm::vec3(0.0f, 0.0f, 1.0f));
    // -Z (back)
    buildFace(vertices, indices, glm::vec3(s, -s, -s), glm::vec3(-s, -s, -s),
              glm::vec3(-s, s, -s), glm::vec3(s, s, -s),
              glm::vec3(0.0f, 0.0f, -1.0f));

    Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setIndices(indices);
    mesh.uploadToGPU();
    return mesh;
}

Mesh createFullscreenQuad()
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    glm::vec3 ndc_bl = glm::vec3(-1.0f, -1.0f, 0.0f);
    glm::vec3 ndc_br = glm::vec3(1.0f, -1.0f, 0.0f);
    glm::vec3 ndc_tr = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 ndc_tl = glm::vec3(-1.0f, 1.0f, 0.0f);

    glm::vec2 uv_bl = glm::vec2(0.0f, 0.0f);
    glm::vec2 uv_br = glm::vec2(1.0f, 0.0f);
    glm::vec2 uv_tr = glm::vec2(1.0f, 1.0f);
    glm::vec2 uv_tl = glm::vec2(0.0f, 1.0f);

    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);

    vertices.push_back({ndc_bl, uv_bl, normal}); // 0
    vertices.push_back({ndc_br, uv_br, normal}); // 1
    vertices.push_back({ndc_tr, uv_tr, normal}); // 2
    vertices.push_back({ndc_tl, uv_tl, normal}); // 3

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setIndices(indices);
    mesh.uploadToGPU();
    return mesh;
}

} // namespace primitives
