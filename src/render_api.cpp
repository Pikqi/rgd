#include "logger.h"
#include <GLFW/glfw3.h>
#include <render_api.h>

void RenderAPI::clear()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
void RenderAPI::init(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderAPI::updateViewPort(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    LOG_INFO("Viewport updated: {} {}", width, height);
}
