#pragma once
#include <glm/glm.hpp>

class RenderAPI
{

  public:
    static void init(unsigned int width, unsigned int height);
    static void shutdown();

    static void setClearColor(float r, float g, float b, float a = 1.0f);
    static void clear();

    static void setViewport(unsigned int width, unsigned int height);
    static void enableDepthTest(bool enable);
    static void enableBlending(bool enable);

    RenderAPI() = delete;
};
