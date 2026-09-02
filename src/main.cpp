#include "camera3d.h"
#include "clouds.h"
#include "game.h"
#include "post_process_chain.h"
#include "sky.h"
#include "terrain.h"
#include "water.h"
#include <GLFW/glfw3.h>
#include "glm/ext/vector_float3.hpp"
#include "render_api.h"
#include "render_context.h"
#include "renderer.h"
#include "resource_manager.h"
#include "shader.h"
#include <cstdint>
#include <algorithm>
#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <stb_image.h>

#include <logger.h>
#include <spdlog/spdlog.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCREEN_WIDTH  = 1720;
const unsigned int SCREEN_HEIGHT = 890;

Game     game(SCREEN_WIDTH, SCREEN_HEIGHT);
Camera3D camera(glm::vec3(64.0f, 15.0f, 64.0f));

bool show_demo_window   = false;
bool mouse_look_enabled = true;

const int   TERRAIN_SIZE = 512;
NoiseParams noiseParams;

glm::mat4 projection;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view  = glm::mat4(1.0f);

glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 lightPos   = glm::vec3(64.0f, 80.0f, 64.0f);
glm::vec3 sunDir     = glm::normalize(glm::vec3(0.5f, 0.7f, 0.3f));
glm::vec3 sunColor   = glm::vec3(1.0f, 0.95f, 0.85f);

static PostProcessChain* g_pipeline = nullptr;
static Clouds*           g_clouds   = nullptr;
static Water*            g_water    = nullptr;
Shader                   tonemap_shader;
Shader                   vignette_shader;
Shader                   crt_shader;
float                    exposure = 1.0f;

float vignetteAmount = 0.335f;
float vignetteRadius = 1.01;
float vignetteSoft   = 0.4f;

bool  crtEnabled           = true;
float crtCurvature         = 0.08f;
float crtScanlineIntensity = 0.25f;
float crtScanlineCount     = 480.0f;
float crtAberration        = 0.0015f;
float crtVignette          = 0.25f;
float crtBrightness        = 1.15f;

bool drawClouds  = true;
bool drawWater   = true;
bool drawTerrain = true;

int main(int argc, char* argv[])
{
    stbi_set_flip_vertically_on_load(true);
    Logger::init();
    LOG_INFO("GLFW Init");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, true);
    glfwWindowHint(GLFW_FLOATING, true);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                                          "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    RenderContext render_context = RenderContext(window);
    render_context.init();

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    RenderAPI::init(SCREEN_WIDTH, SCREEN_HEIGHT);
    RenderAPI::setClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    Renderer renderer;

    static PostProcessChain pipeline(SCREEN_WIDTH, SCREEN_HEIGHT);
    g_pipeline = &pipeline;

    LOG_INFO("IMGUI Init");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    float main_scale =
        ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    LOG_INFO("IMGUI Init end");

    game.Init();

    Shader& terrain_shader =
        ResourceManager::LoadShader("shaders/terrain/vertex.glsl",
                                    "shaders/terrain/fragment.glsl", "terrain");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    tonemap_shader = ResourceManager::LoadShader(
        "shaders/postprocess/quad.vert", "shaders/postprocess/tonemap.frag",
        "tonemap");
    vignette_shader = ResourceManager::LoadShader(
        "shaders/postprocess/quad.vert", "shaders/postprocess/vignette.frag",
        "vignette");
    crt_shader = ResourceManager::LoadShader(
        "shaders/postprocess/quad.vert", "shaders/postprocess/crt.frag", "crt");
    Shader passthrough_shader = ResourceManager::LoadShader(
        "shaders/postprocess/quad.vert",
        "shaders/postprocess/passthrough.frag", "passthrough");

    pipeline.addEffect(&tonemap_shader, "Tonemap");
    pipeline.addEffect(&vignette_shader, "Vignette");

    Sky sky;

    Clouds clouds(SCREEN_WIDTH, SCREEN_HEIGHT);
    g_clouds = &clouds;

    Water water(SCREEN_WIDTH, SCREEN_HEIGHT);
    g_water = &water;

    Terrain terrain({TERRAIN_SIZE, TERRAIN_SIZE}, noiseParams);
    while (!glfwWindowShouldClose(window))
    {
        int width  = 0;
        int height = 0;
        glfwGetWindowSize(window, &width, &height);
        projection = glm::perspective(glm::radians(45.0f),
                                      static_cast<float>(width) /
                                          static_cast<float>(height),
                                      0.1f, 500.0f);

        view = camera.getViewMatrix();

        float currentFrame = glfwGetTime();
        deltaTime          = currentFrame - lastFrame;
        lastFrame          = currentFrame;
        glfwPollEvents();

        glm::vec2 cam_movement = {0.0f, 0.0f};
        if (game.Keys[GLFW_KEY_W])
            cam_movement.y += 1.0f;
        if (game.Keys[GLFW_KEY_S])
            cam_movement.y -= 1.0f;
        if (game.Keys[GLFW_KEY_A])
            cam_movement.x -= 1.0f;
        if (game.Keys[GLFW_KEY_D])
            cam_movement.x += 1.0f;
        if (glm::length(cam_movement) > 0.0f)
            camera.moveCamera(cam_movement, deltaTime);

        game.ProcessInput(deltaTime);
        game.Update(deltaTime);

        pipeline.beginScene();

        // The sky is the first thing drawn so terrain overdraws on top via
        // depth.
        sky.draw(camera, projection, sunDir, sunColor);

        terrain_shader.setMatrix4("projection", projection);
        terrain_shader.setMatrix4("view", view);
        terrain_shader.setMatrix4("model", model);
        terrain_shader.setVector4f("lightColor", lightColor);
        terrain_shader.setVector3f("lightPos", lightPos);
        terrain_shader.setVector3f("camPos", camera.position);
        if (drawTerrain)
            renderer.draw(terrain.getMesh(), terrain_shader);

        if (drawWater)
        {
            water.draw(camera, projection, sunDir, sunColor, pipeline.scene(),
                       static_cast<float>(glfwGetTime()));
        }

        if (drawClouds)
        {
            clouds.draw(camera, projection, sunDir, sunColor, pipeline.scene(),
                        static_cast<float>(glfwGetTime()));
        }

        tonemap_shader.setFloat("uExposure", exposure);
        vignette_shader.setFloat("uIntensity", vignetteAmount);
        vignette_shader.setFloat("uRadius", vignetteRadius);
        vignette_shader.setFloat("uSoftness", vignetteSoft);

        if (crtEnabled)
        {
            crt_shader.setFloat("uCurvature", crtCurvature);
            crt_shader.setFloat("uScanlineIntensity", crtScanlineIntensity);
            crt_shader.setFloat("uScanlineCount", crtScanlineCount);
            crt_shader.setFloat("uAberration", crtAberration);
            crt_shader.setFloat("uVignette", crtVignette);
            crt_shader.setFloat("uBrightness", crtBrightness);
            pipeline.run(crt_shader);
        }
        else
        {
            pipeline.run(passthrough_shader);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Controls");
            // ImGui::SetWindowPos({0.0f, 0.0f});
            // ImGui::SetWindowSize({400.0f, static_cast<float>(game.Height)});

            ImGui::Checkbox("Draw clouds (1)", &drawClouds);
            ImGui::Checkbox("Draw water (2)", &drawWater);
            ImGui::Checkbox("Draw terrain (3)", &drawTerrain);
            if (ImGui::Button("Recompile shaders"))
            {
                ResourceManager::ReCompileShaders();
            }

            if (ImGui::CollapsingHeader("Terrain",
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                bool paramsChanged = false;
                paramsChanged |= ImGui::SliderFloat(
                    "Octaves", &noiseParams.octaves, 1.0f, 8.0f, "%.0f");
                paramsChanged |= ImGui::SliderFloat(
                    "Frequency", &noiseParams.frequency, 0.001f, 0.1f, "%.3f");
                paramsChanged |= ImGui::SliderFloat(
                    "Amplitude", &noiseParams.amplitude, 1.0f, 100.0f, "%.1f");
                paramsChanged |=
                    ImGui::SliderFloat("Persistence", &noiseParams.persistence,
                                       0.1f, 0.9f, "%.2f");
                paramsChanged |= ImGui::SliderFloat(
                    "Lacunarity", &noiseParams.lacunarity, 1.0f, 4.0f, "%.2f");

                if (paramsChanged || ImGui::Button("Regenerate"))
                {
                    terrain.regenerate(noiseParams);
                }

                ImGui::Separator();

                ImGui::Text("Camera Position:");
                ImGui::Text("X: %.1f", camera.position.x);
                ImGui::Text("Y: %.1f", camera.position.y);
                ImGui::Text("Z: %.1f", camera.position.z);
                ImGui::Text("YAW: %.1f", camera.yaw);
                ImGui::Text("PITCH: %.1f", camera.pitch);

                ImGui::Text("Controls:");
                ImGui::Text("- WASD to move");
                ImGui::Text("- Mouse to look around");
            }

            if (ImGui::CollapsingHeader("Sky"))
            {
                static float azimuth   = 33.0f;
                static float elevation = 45.0f;
                ImGui::SliderFloat("Azimuth", &azimuth, 0.0f, 360.0f);
                ImGui::SliderFloat("Elevation", &elevation, -10.0f, 90.0f);

                float az = glm::radians(azimuth);
                float el = glm::radians(elevation);
                sunDir   = glm::normalize(
                    glm::vec3(cos(el) * cos(az), sin(el), cos(el) * sin(az)));

                ImGui::ColorEdit3("Sun color", &sunColor.x);
                ImGui::ColorEdit3("Zenith", &sky.zenithColor.x);
                ImGui::ColorEdit3("Horizon", &sky.horizonColor.x);
            }

            if (ImGui::CollapsingHeader("Water"))
            {
                ImGui::SliderFloat("Sea level", &water.seaLevel, -30.0f, 30.0f,
                                   "%.1f");
                ImGui::SliderFloat("Wave amplitude", &water.waveAmplitude, 0.0f,
                                   2.0f, "%.2f");
                ImGui::SliderFloat("Shininess", &water.shininess, 8.0f, 512.0f,
                                   "%.0f");
                ImGui::ColorEdit3("Water color", &water.waterColor.x);
                ImGui::ColorEdit3("Horizon tint", &water.horizonColor.x);
                ImGui::Separator();
                ImGui::SliderFloat("Distortion strength",
                                   &water.distortionStrength, 0.0f, 0.1f,
                                   "%.3f");
                ImGui::SliderFloat("Depth darkness", &water.depthDarkness, 0.0f,
                                   1.0f, "%.2f");
                ImGui::SliderFloat("Max water depth", &water.maxWaterDepth,
                                   1.0f, 100.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Clouds"))
            {
                ImGui::SliderFloat("Layer height", &clouds.layerStart, 10.0f,
                                   60.0f, "%.1f");
                ImGui::SliderFloat("Cloud thickness", &clouds.layerHeight, 2.0f,
                                   30.0f, "%.1f");
                ImGui::SliderFloat("Coverage", &clouds.coverage, 0.0f, 1.0f,
                                   "%.2f");
                ImGui::SliderFloat("Wind speed", &clouds.windSpeed, 0.0f, 5.0f,
                                   "%.2f");
                ImGui::SliderInt("Primary steps", &clouds.stepCount, 16, 256);
                ImGui::Checkbox("Enable offset start",
                                &clouds.enableOffsetStart);
                ImGui::TextWrapped(
                    "Sun color / direction are controlled from the Sky panel.");
            }

            if (ImGui::CollapsingHeader("Stats"))
            {
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                            1000.0f / io.Framerate, io.Framerate);

                ImGui::Value("Mouse X", game.mouse_pos.x);
                ImGui::SameLine();
                ImGui::Value("Mouse Y", game.mouse_pos.y);
            }

            if (ImGui::CollapsingHeader("Post FX"))
            {
                ImGui::SliderFloat("Exposure", &exposure, 0.0f, 3.0f);
                ImGui::SliderFloat("Vignette intensity", &vignetteAmount, 0.0f,
                                   1.0f);
                ImGui::SliderFloat("Vignette radius", &vignetteRadius, 0.1f,
                                   1.5f);
                ImGui::SliderFloat("Vignette softness", &vignetteSoft, 0.0f,
                                   1.0f);

                ImGui::Separator();
                ImGui::Checkbox("CRT", &crtEnabled);
                ImGui::SliderFloat("CRT curvature", &crtCurvature, 0.0f, 0.4f);
                ImGui::SliderFloat("CRT scanlines", &crtScanlineIntensity, 0.0f,
                                   1.0f);
                ImGui::SliderFloat("CRT scanline count", &crtScanlineCount,
                                   64.0f, 1080.0f, "%.0f");
                ImGui::SliderFloat("CRT aberration", &crtAberration, 0.0f,
                                   0.01f, "%.4f");
                ImGui::SliderFloat("CRT vignette", &crtVignette, 0.0f, 1.0f);
                ImGui::SliderFloat("CRT brightness", &crtBrightness, 0.5f,
                                   2.0f);
            }

            if (ImGui::CollapsingHeader("Pipeline Debug"))
            {
                ImGui::Text("Scene");
                ImGui::Image((ImTextureID)(intptr_t)pipeline.scene().colorTex(),
                             ImVec2(256, 144), ImVec2(0.0f, 1.0f),
                             ImVec2(1.0f, 0.0f));
                for (const auto& e : pipeline.effects())
                {
                    ImGui::Separator();
                    ImGui::Text("%s", e->name());
                    ImGui::Image((ImTextureID)(intptr_t)e->outputTex(),
                                 ImVec2(256, 144), ImVec2(0.0f, 1.0f),
                                 ImVec2(1.0f, 0.0f));
                }
            }

            if (ImGui::CollapsingHeader("Camera"))
            {
                ImGui::Text("World position");
                ImGui::Text("X: %.3f", camera.position.x);
                ImGui::Text("Y: %.3f", camera.position.y);
                ImGui::Text("Z: %.3f", camera.position.z);
            }

            ImGui::End();
        }

        game.Render();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        render_context.swapBuffers();
    }

    ResourceManager::Clear();
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        show_demo_window = !show_demo_window;
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        game.debug_mode = !game.debug_mode;

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        drawClouds = !drawClouds;
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        drawWater = !drawWater;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        drawTerrain = !drawTerrain;

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        ResourceManager::ReCompileShaders();
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            game.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            game.Keys[key] = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    static double last_mouse_posx = -1;
    static double last_mouse_posy = -1;

    if (last_mouse_posx != -1 && mouse_look_enabled)
    {
        const double offset_x = last_mouse_posx - xpos;
        const double offset_y = last_mouse_posy - ypos;

        camera.mouseMoveCamera(offset_x, offset_y);
    }

    last_mouse_posx = xpos;
    last_mouse_posy = ypos;

    game.mouse_pos = {xpos, ypos};
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        mouse_look_enabled = !mouse_look_enabled;

    if (action == GLFW_PRESS)
        game.Keys[button] = true;
    else if (action == GLFW_RELEASE)
        game.Keys[button] = false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    RenderAPI::setViewport(width, height);
    if (g_pipeline)
        g_pipeline->resize(width, height);
    if (g_clouds)
        g_clouds->resize(width, height);
    if (g_water)
        g_water->resize(width, height);
    game.UpdateScreenSize(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.move_speed += yoffset * 2.0f;
    camera.move_speed = std::clamp(camera.move_speed, 1.0f, 200.0f);
}
