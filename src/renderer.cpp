#include "renderer.h"
#include "shader.h"
#include <rgd.h>

void Renderer::Draw(const VertexBuffer& vb, const VertexArray& va,
                    const unsigned int verticies_count,
                    const Shader&      shader) const
{
    vb.bind();
    va.bind();
    shader.use();

    GLCALL(glDrawElements(GL_TRIANGLES, verticies_count, GL_UNSIGNED_INT, 0));
}
void Renderer::Clear() {}
