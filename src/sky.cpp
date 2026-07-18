#include "sky.h"

#include "primitives.h"
#include "resource_manager.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_inverse.hpp>

Sky::Sky()
{
    _shader = ResourceManager::LoadShader(
        "shaders/sky/vertex.glsl", "shaders/sky/fragment.glsl", nullptr, "sky");
    _quad = primitives::createFullscreenQuad();
}

void Sky::draw(const Camera3D& camera, const glm::mat4& projection,
               const glm::vec3& sunDir, const glm::vec3& sunColor)
{
    glm::mat4 invVP = glm::inverse(projection * camera.getViewMatrix());

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);

    _shader.use();
    _shader.setMatrix4("uInvViewProj", invVP);
    _shader.setVector3f("uCameraPos", camera.position);
    _shader.setVector3f("uZenithColor", zenithColor);
    _shader.setVector3f("uHorizonColor", horizonColor);
    _shader.setVector3f("uSunDir", sunDir);
    _shader.setVector3f("uSunColor", sunColor);
    _quad.bind();
    glDrawArrays(GL_TRIANGLES, 0, _quad.getVertexCount());

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
}
