#pragma once

#include "index_buffer.h"
#include "mesh.h"
#include "shader.h"
#include "vertex_array.h"

class Renderer
{
  public:
    Renderer() = default;

    void clear() const;

    void draw(const Mesh& mesh, const Shader& shader) const;
    void draw(const VertexArray& va, const IndexBuffer& ib,
              const Shader& shader) const;

  private:
};
