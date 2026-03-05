#include "logger.h"
#include <render_context.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

RenderContext::RenderContext(GLFWwindow* window) : _window_handle(window)
{
    if (!window)
    {
        LOG_FATAL("Window handle is null");
    }
};
int RenderContext::init()
{
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_FATAL("Failed to initialize GLAD");
        return -1;
    }
    LOG_INFO("GLAD Loaded");
    return 0;
}
void RenderContext::swapBuffers()
{
    glfwSwapBuffers(_window_handle);
}
