#pragma once

class RenderAPI
{

  public:
    static void clear();
    static void init(unsigned int width, unsigned int height);
    static void updateViewPort(unsigned int width, unsigned int height);
    static void setBlending(bool enable);

    RenderAPI() = delete;
};
