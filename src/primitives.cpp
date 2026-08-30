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

    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 1.0f);

    // Two triangles, 6 vertices, no shared vertices.
    // Triangle 1: BL, BR, TR  (bottom-right half)
    vertices.push_back(
        {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), normal}); // 0
    vertices.push_back(
        {glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), normal}); // 1
    vertices.push_back(
        {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), normal}); // 2
    // Triangle 2: BL, TR, TL  (top-left half)
    vertices.push_back(
        {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), normal}); // 3
    vertices.push_back(
        {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), normal}); // 4
    vertices.push_back(
        {glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), normal}); // 5

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(4);
    indices.push_back(5);

    Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setIndices(indices);
    mesh.uploadToGPU();
    return mesh;
}

Mesh createPlaneXZ(float size)
{
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;

    const float     h = 0.5f * size;
    const glm::vec3 normal(0.0f, 1.0f, 0.0f);

    vertices.push_back(
        {glm::vec3(-h, 0.0f, -h), glm::vec2(0.0f, 0.0f), normal}); // 0 BL
    vertices.push_back(
        {glm::vec3(h, 0.0f, -h), glm::vec2(1.0f, 0.0f), normal}); // 1 BR
    vertices.push_back(
        {glm::vec3(h, 0.0f, h), glm::vec2(1.0f, 1.0f), normal}); // 2 TR
    vertices.push_back(
        {glm::vec3(-h, 0.0f, h), glm::vec2(0.0f, 1.0f), normal}); // 3 TL

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
