#include "renderer.h"
#include "index_buffer.h"
#include "logger.h"
#include "mesh.h"
#include "render_api.h"
#include "shader.h"
#include <rgd.h>

void Renderer::clear() const
{
    RenderAPI::clear();
}

void Renderer::draw(const Mesh& mesh, const Shader& shader) const
{
    if (!mesh.isUploaded())
    {
        LOG_WARN("Mesh: Trying to draw before uploading to GPU");
        return;
    }

    mesh.bind();
    shader.use();
    GLCALL(
        glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib,
                    const Shader& shader) const
{
    va.bind();
    ib.bind();
    shader.use();
    GLCALL(glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, 0));
}
