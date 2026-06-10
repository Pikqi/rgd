
#pragma once

#include "index_buffer.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"

class Renderer
{
  public:
    void Draw(const VertexBuffer& vb, const VertexArray& va,
              const IndexBuffer& ib, const Shader& shader) const;
    void Clear();

  private:
};
