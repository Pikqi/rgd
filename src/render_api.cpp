#include "logger.h"
#include <glm/glm.hpp>
#include <rgd.h>
#include <GLFW/glfw3.h>
#include <render_api.h>

namespace {
glm::vec4 g_clear_color   = glm::vec4(0.0f, 0.0f, 0.3f, 1.0f);
bool      g_depth_test_on = true;
bool      g_blending_on   = true;
} // namespace

void RenderAPI::init(unsigned int width, unsigned int height)
{
    GLCALL(glViewport(0, 0, width, height));

    if (g_depth_test_on)
    {
        GLCALL(glEnable(GL_DEPTH_TEST));
    }
    else
    {
        GLCALL(glDisable(GL_DEPTH_TEST));
    }

    if (g_blending_on)
    {
        GLCALL(glEnable(GL_BLEND));
        GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    }
    else
    {
        GLCALL(glDisable(GL_BLEND));
    }

    GLCALL(glClearColor(g_clear_color.r, g_clear_color.g, g_clear_color.b,
                        g_clear_color.a));
}

void RenderAPI::shutdown()
{
    GLCALL(glDisable(GL_DEPTH_TEST));
    GLCALL(glDisable(GL_BLEND));
}

void RenderAPI::setClearColor(float r, float g, float b, float a)
{
    g_clear_color = glm::vec4(r, g, b, a);
    GLCALL(glClearColor(r, g, b, a));
}

void RenderAPI::clear()
{
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void RenderAPI::setViewport(unsigned int width, unsigned int height)
{
    GLCALL(glViewport(0, 0, width, height));
    LOG_INFO("Viewport updated: {} {}", width, height);
}

void RenderAPI::enableDepthTest(bool enable)
{
    g_depth_test_on = enable;
    if (enable)
    {
        GLCALL(glEnable(GL_DEPTH_TEST));
    }
    else
    {
        GLCALL(glDisable(GL_DEPTH_TEST));
    }
}

void RenderAPI::enableBlending(bool enable)
{
    g_blending_on = enable;
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
