#include "renderer.h"
#include "index_buffer.h"
#include "shader.h"
#include <rgd.h>

void Renderer::Draw(const VertexBuffer& vb, const VertexArray& va,
                    const IndexBuffer& ib, const Shader& shader) const
{
    vb.bind();
    va.bind();
    ib.bind();
    shader.use();

    GLCALL(glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, 0));
}
void Renderer::Clear()
{
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
