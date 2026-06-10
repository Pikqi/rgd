#include "camera3d.h"
#include "game.h"
#include <GLFW/glfw3.h>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "index_buffer.h"
#include "resource_manager.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include <cstdint>
#include <imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <render_context.h>
#include <render_api.h>
#include <rgd.h>
#include <renderer.h>
#include <stb_image.h>

#include <logger.h>
#include <spdlog/spdlog.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCREEN_WIDTH = 1720;
const unsigned int SCREEN_HEIGHT = 890;

Game game(SCREEN_WIDTH, SCREEN_HEIGHT);
Camera3D camera(glm::vec3(0.0f, 2.0f, -5.0f));

bool show_demo_window = false;

glm::mat4 projection;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
glm::mat4 lightModel = glm::mat4(1.0f);
glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 pyramidModel = glm::mat4(1.0f);
glm::vec3 floorPos = glm::vec3(0.0f, 0.0f, -1.0f);
glm::mat4 floorModel = glm::mat4(1.0f);

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
    Renderer renderer;

    LOG_INFO("IMGUI Init");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    style.ScaleAllSizes(2);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    LOG_INFO("IMGUI Init end");

    game.Init();

    Shader default_shader = ResourceManager::LoadShader(
        "shaders/default/vertex.glsl", "shaders/default/fragment.glsl", NULL, "default");

    Shader specular_shader = ResourceManager::LoadShader(
        "shaders/specular_map/vertex.glsl",
        "shaders/specular_map/fragment.glsl", NULL, "specular");

    Shader light_shader = ResourceManager::LoadShader(
        "shaders/light/vertex.glsl", "shaders/light/fragment.glsl", NULL, "light");

    GLfloat light_vertices[] = {
        -0.1f, -0.1f, 0.1f,  -0.1f, -0.1f, -0.1f, 0.1f, -0.1f,
        -0.1f, 0.1f,  -0.1f, 0.1f,  -0.1f, 0.1f,  0.1f, -0.1f,
        0.1f,  -0.1f, 0.1f,  0.1f,  -0.1f, 0.1f,  0.1f, 0.1f};

    GLuint light_indices[] = {0, 1, 2, 0, 2, 3, 0, 4, 7, 0, 7, 3, 3, 7, 6, 3, 6, 2,
                              2, 6, 5, 2, 5, 1, 1, 5, 4, 1, 4, 0, 4, 5, 6, 4, 6, 7};

    VertexArray light_va;
    VertexBuffer light_vb(light_vertices, sizeof(light_vertices));
    IndexBuffer light_ib(light_indices, 36);
    VertexBufferLayout light_va_layout;
    light_va_layout.push(GL_FLOAT, 3);
    light_va.add_buffer(light_vb, light_va_layout);

    lightModel = glm::translate(lightModel, lightPos);

    auto brick_texture = ResourceManager::LoadTexture("res/brick.png", true, "brick");
    brick_texture->setRepeat(true);

    auto planks_texture = ResourceManager::LoadTexture("res/planks.png", true, "planks");
    planks_texture->setRepeat(true);

    auto planks_texture_spec = ResourceManager::LoadTexture("res/planksSpec.png", true, "planks_spec");
    planks_texture_spec->setRepeat(true);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        int width = 0;
        int height = 0;
        glfwGetWindowSize(window, &width, &height);
        projection = glm::perspective(glm::radians(45.0f),
                                      static_cast<float>(width) / static_cast<float>(height),
                                      0.1f, 100.0f);

        view = camera.getViewMatrix();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
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

        RenderAPI::clear();

        default_shader.setMatrix4("projection", projection);
        default_shader.setMatrix4("view", view);
        default_shader.setMatrix4("model", model);
        default_shader.setVector4f("lightColor", lightColor);
        default_shader.setVector3f("lightPos", lightPos);
        default_shader.setVector3f("camPos", camera.position);
        brick_texture->bind(0);

        light_shader.setMatrix4("projection", projection);
        light_shader.setMatrix4("model", lightModel);
        light_shader.setMatrix4("view", camera.getViewMatrix());
        light_shader.setVector4f("lightColor", lightColor);

        light_va.bind();
        light_vb.bind();
        light_ib.bind();
        light_shader.use();
        GLCALL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Terrain Controls");
            ImGui::Text("Terrain generation coming soon...");
            ImGui::Text("Controls:");
            ImGui::Text("- WASD to move camera");
            ImGui::Text("- Mouse to look around");
            ImGui::End();
        }

        {
            ImGui::Begin("Hello, world!");
            ImGui::SetWindowPos({static_cast<float>(game.Width) - 400.0f, 0.0f});
            ImGui::SetWindowSize({400.0f, static_cast<float>(game.Height)});
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);

            ImGui::Value("Mouse X", game.mouse_pos.x);
            ImGui::SameLine();
            ImGui::Value("Mouse Y", game.mouse_pos.y);
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
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

    if (last_mouse_posx != -1)
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
    if (action == GLFW_PRESS)
        game.Keys[button] = true;
    else if (action == GLFW_RELEASE)
        game.Keys[button] = false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    RenderAPI::updateViewPort(width, height);
    game.UpdateScreenSize(width, height);
}