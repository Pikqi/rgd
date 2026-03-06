#include "logger.h"
#include <rgd.h>
#include <GLFW/glfw3.h>
#include <render_api.h>

void RenderAPI::clear()
{
    GLCALL(glClearColor(0.0f, 0.0f, 0.3f, 1.0f));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
void RenderAPI::init(unsigned int width, unsigned int height)
{
    GLCALL(glViewport(0, 0, width, height));

    GLCALL(glEnable(GL_DEPTH_TEST));
}
void RenderAPI::setBlending(bool enable)
{
    if (enable)
    {

        GLCALL(glEnable(GL_BLEND));
        GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }
    else
    {
        GLCALL(glDisable(GL_BLEND));
    }
}

void RenderAPI::updateViewPort(unsigned int width, unsigned int height)
{
    GLCALL(glViewport(0, 0, width, height));
    LOG_INFO("Viewport updated: {} {}", width, height);
}
