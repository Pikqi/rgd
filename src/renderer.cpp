#include "renderer.h"
#include "shader.h"
#include <rgd.h>

void Renderer::Draw(const VertexBuffer& vb, const VertexArray& va,
                    const Shader& shader) const
{
    vb.bind();
    va.bind();
    shader.use();

    GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}
void Renderer::Clear() {}
