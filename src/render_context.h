#pragma once
#include "imgui_impl_glfw.h"

class RenderContext
{
  public:
    RenderContext(GLFWwindow* window);
    int  init();
    void swapBuffers();

  private:
    GLFWwindow* _window_handle;
};
