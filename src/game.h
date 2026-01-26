#pragma once

#include "glm/ext/vector_float2.hpp"
#include "post_processor.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

class Game
{
  public:
    bool               Keys[1024];
    unsigned int       Width, Height;
    PostProcessor*     postProcessor;
    glm::vec2          mouse_pos;
    glm::vec4          projeciton;
    glm::vec2          movement_input;
    bool               debug_mode = false;

		static float shake_strenght;
		static float shake_duration;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    void UpdateScreenSize(int w, int h);

};
