
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
class Renderer
{
  public:
    void Draw(const VertexBuffer& vb, const VertexArray& va,
              const unsigned int verticies_count, const Shader& shader) const;
    void Clear();

  private:
};
