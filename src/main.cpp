#include "camera3d.h"
#include "clouds.h"
#include "game.h"
#include "post_process_chain.h"
#include "sky.h"
#include "terrain.h"
#include <GLFW/glfw3.h>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "primitives.h"
#include "render_api.h"
#include "render_context.h"
#include "renderer.h"
#include "resource_manager.h"
#include "shader.h"
#include <cstdint>
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

glm::vec4 lightColor   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 lightPos     = glm::vec3(64.0f, 80.0f, 64.0f);
glm::mat4 lightModel   = glm::mat4(1.0f);
glm::vec3 sunDir       = glm::normalize(glm::vec3(0.5f, 0.7f, 0.3f));
glm::vec3 sunColor     = glm::vec3(1.0f, 0.95f, 0.85f);
glm::vec3 pyramidPos   = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 pyramidModel = glm::mat4(1.0f);
glm::vec3 floorPos     = glm::vec3(0.0f, 0.0f, -1.0f);
glm::mat4 floorModel   = glm::mat4(1.0f);

static PostProcessChain* g_pipeline = nullptr;
static Clouds*           g_clouds   = nullptr;
Shader                   tonemap_shader;
Shader                   vignette_shader;
float                    exposure   = 1.0f;
bool                     applyGamma = true;

float vignetteAmount = 0.335f;
float vignetteRadius = 1.01;
float vignetteSoft   = 0.4f;

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
    style.ScaleAllSizes(2);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    LOG_INFO("IMGUI Init end");

    game.Init();

    Shader terrain_shader = ResourceManager::LoadShader(
        "shaders/terrain/vertex.glsl", "shaders/terrain/fragment.glsl", NULL,
        "terrain");

    Terrain terrain({TERRAIN_SIZE, TERRAIN_SIZE}, noiseParams);

    auto brick_texture =
        ResourceManager::LoadTexture("res/brick.png", true, "brick");
    brick_texture->setRepeat(true);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    Shader default_shader = ResourceManager::LoadShader(
        "shaders/default/vertex.glsl", "shaders/default/fragment.glsl", NULL,
        "default");

    Shader specular_shader = ResourceManager::LoadShader(
        "shaders/specular_map/vertex.glsl",
        "shaders/specular_map/fragment.glsl", NULL, "specular");

    Shader light_shader = ResourceManager::LoadShader(
        "shaders/light/vertex.glsl", "shaders/light/fragment.glsl", NULL,
        "light");
    tonemap_shader = ResourceManager::LoadShader(
        "shaders/postprocess/quad.vert", "shaders/postprocess/tonemap.frag",
        nullptr, "tonemap");
    vignette_shader = ResourceManager::LoadShader(
        "shaders/postprocess/quad.vert", "shaders/postprocess/vignette.frag",
        nullptr, "vignette");

    pipeline.addEffect(&tonemap_shader, "Tonemap");
    pipeline.addEffect(&vignette_shader, "Vignette");

    Sky sky;

    Clouds clouds(SCREEN_WIDTH, SCREEN_HEIGHT);
    g_clouds = &clouds;

    Mesh light_mesh = primitives::createCube();
    lightModel      = glm::translate(lightModel, lightPos);

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

        // --- Sky pass: fullscreen quad into the scene FBO ---
        // The sky is the first thing drawn so terrain overdraws on top via
        // depth.
        sky.draw(camera, projection, sunDir, sunColor);

        clouds.draw(camera, projection, sunDir, sunColor, pipeline.scene(),
                    static_cast<float>(glfwGetTime()));

        terrain_shader.setMatrix4("projection", projection);
        terrain_shader.setMatrix4("view", view);
        terrain_shader.setMatrix4("model", model);
        terrain_shader.setVector4f("lightColor", lightColor);
        terrain_shader.setVector3f("lightPos", lightPos);
        terrain_shader.setVector3f("camPos", camera.position);
        renderer.draw(terrain.getMesh(), terrain_shader);

        light_shader.setMatrix4("projection", projection);
        light_shader.setMatrix4("model", lightModel);
        light_shader.setMatrix4("view", camera.getViewMatrix());
        light_shader.setVector4f("lightColor", lightColor);
        renderer.draw(light_mesh, light_shader);

        tonemap_shader.setFloat("uExposure", exposure);
        tonemap_shader.setInteger("uApplyGamma", applyGamma);
        vignette_shader.setFloat("uIntensity", vignetteAmount);
        vignette_shader.setFloat("uRadius", vignetteRadius);
        vignette_shader.setFloat("uSoftness", vignetteSoft);
        pipeline.run(vignette_shader);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Controls");
            // ImGui::SetWindowPos({0.0f, 0.0f});
            ImGui::SetWindowSize({400.0f, static_cast<float>(game.Height)});

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
                ImGui::Checkbox("Gamma correct", &applyGamma);
                ImGui::SliderFloat("Vignette intensity", &vignetteAmount, 0.0f,
                                   1.0f);
                ImGui::SliderFloat("Vignette radius", &vignetteRadius, 0.1f,
                                   1.5f);
                ImGui::SliderFloat("Vignette softness", &vignetteSoft, 0.0f,
                                   1.0f);
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
    game.UpdateScreenSize(width, height);
}
