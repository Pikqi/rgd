#include <game.h>
#include <GLFW/glfw3.h>
#include <debug.h>
#include <resource_manager.h>

void Game::UpdateScreenSize(int w, int h)
{
    // Width  = w;
    // Height = h;
    // for (auto [name, shader] : ResourceManager::shaders)
    // {
    //     auto projection = glm::ortho(0.0f, (float)Width, (float)Height,
    //     0.0f); shader.Use(); shader.SetMatrix4("projection", projection);
    // }
}

Game::Game(unsigned int width, unsigned int height)
{
    Width  = width;
    Height = height;
    for (int i = 0; i < 1024; i++)
        Keys[i] = false;
}
Game::~Game() {}

void Game::Init() {}

void Game::Update(float dt) {}

void Game::ProcessInput(float dt) {}

void Game::Render()
{

    if (debug_mode)
    {
        debugFlush();
    }
    else
    {
        debugClear();
    }
}
