#include "camera3d.h"
#include "game.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
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

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode);

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 1720;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 890;

Game game(SCREEN_WIDTH, SCREEN_HEIGHT);

Camera3D camera(glm::vec3(0.0f, 0.0f, -2.0f));

bool show_demo_window = false;

GLfloat light_vertices[] = { //     COORDINATES     //
    -0.1f, -0.1f, 0.1f,  -0.1f, -0.1f, -0.1f, 0.1f, -0.1f,
    -0.1f, 0.1f,  -0.1f, 0.1f,  -0.1f, 0.1f,  0.1f, -0.1f,
    0.1f,  -0.1f, 0.1f,  0.1f,  -0.1f, 0.1f,  0.1f, 0.1f};

GLuint light_indices[] = {0, 1, 2, 0, 2, 3, 0, 4, 7, 0, 7, 3, 3, 7, 6, 3, 6, 2,
                          2, 6, 5, 2, 5, 1, 1, 5, 4, 1, 4, 0, 4, 5, 6, 4, 6, 7};

// Vertices coordinates
GLfloat pyramid_vertices[] = {
    //     COORDINATES     /        COLORS          /    TexCoord   / NORMALS //
    -0.5f, 0.0f, 0.5f,  0.83f, 0.70f, 0.44f,
    0.0f,  0.0f, 0.0f,  -1.0f, 0.0f, // Bottom side
    -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f,
    0.0f,  5.0f, 0.0f,  -1.0f, 0.0f, // Bottom side
    0.5f,  0.0f, -0.5f, 0.83f, 0.70f, 0.44f,
    5.0f,  5.0f, 0.0f,  -1.0f, 0.0f, // Bottom side
    0.5f,  0.0f, 0.5f,  0.83f, 0.70f, 0.44f,
    5.0f,  0.0f, 0.0f,  -1.0f, 0.0f, // Bottom side

    -0.5f, 0.0f, 0.5f,  0.83f, 0.70f, 0.44f,
    0.0f,  0.0f, -0.8f, 0.5f,  0.0f, // Left Side
    -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f,
    5.0f,  0.0f, -0.8f, 0.5f,  0.0f, // Left Side
    0.0f,  0.8f, 0.0f,  0.92f, 0.86f, 0.76f,
    2.5f,  5.0f, -0.8f, 0.5f,  0.0f, // Left Side

    -0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f,
    5.0f,  0.0f, 0.0f,  0.5f,  -0.8f, // Non-facing side
    0.5f,  0.0f, -0.5f, 0.83f, 0.70f, 0.44f,
    0.0f,  0.0f, 0.0f,  0.5f,  -0.8f, // Non-facing side
    0.0f,  0.8f, 0.0f,  0.92f, 0.86f, 0.76f,
    2.5f,  5.0f, 0.0f,  0.5f,  -0.8f, // Non-facing side

    0.5f,  0.0f, -0.5f, 0.83f, 0.70f, 0.44f,
    0.0f,  0.0f, 0.8f,  0.5f,  0.0f, // Right side
    0.5f,  0.0f, 0.5f,  0.83f, 0.70f, 0.44f,
    5.0f,  0.0f, 0.8f,  0.5f,  0.0f, // Right side
    0.0f,  0.8f, 0.0f,  0.92f, 0.86f, 0.76f,
    2.5f,  5.0f, 0.8f,  0.5f,  0.0f, // Right side

    0.5f,  0.0f, 0.5f,  0.83f, 0.70f, 0.44f,
    5.0f,  0.0f, 0.0f,  0.5f,  0.8f, // Facing side
    -0.5f, 0.0f, 0.5f,  0.83f, 0.70f, 0.44f,
    0.0f,  0.0f, 0.0f,  0.5f,  0.8f, // Facing side
    0.0f,  0.8f, 0.0f,  0.92f, 0.86f, 0.76f,
    2.5f,  5.0f, 0.0f,  0.5f,  0.8f // Facing side
};

// Indices for vertices order
GLuint pyramid_indices[] = {
    0,  1,  2,  // Bottom side
    0,  2,  3,  // Bottom side
    4,  6,  5,  // Left side
    7,  9,  8,  // Non-facing side
    10, 12, 11, // Right side
    13, 15, 14  // Facing side
};

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

    // OpenGL configuration
    // --------------------
    RenderAPI::init(SCREEN_WIDTH, SCREEN_HEIGHT);
    Renderer renderer;

    // Init IMGUI
    LOG_INFO("IMGUI Init");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |=
    //     ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |=
    //     ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup scaling
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(
        glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(
        main_scale); // Bake a fixed style scale. (until we have a solution for
                     // dynamic style scaling, changing this requires resetting
                     // Style + calling this again)
    style.FontScaleDpi =
        main_scale; // Set initial font scale. (using
                    // io.ConfigDpiScaleFonts=true makes this unnecessary. We
                    // leave both here for documentation purpose)
    style.ScaleAllSizes(2);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init("#version 330");
    bool   show_another_window = false;
    ImVec4 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    LOG_INFO("IMGUI Init end");

    // initialize game
    // ---------------

    game.Init();
    auto texture =
        ResourceManager::LoadTexture("res/awesomeface.png", true, "face");
    auto brick_texture =
        ResourceManager::LoadTexture("res/brick.png", true, "brick");
    brick_texture->setRepeat(true);

    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // float vertices[] = {
    //     0.5f,  0.5f,  0.0f, // top right
    //     0.5f,  -0.5f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f, // bottom left
    //     -0.5f, 0.5f,  0.0f  // top left
    // };
    // unsigned int indices[] = {
    //     0, 1, 3, //
    //     1, 2, 3  //
    // };

    Shader default_shader = ResourceManager::LoadShader(
        "shaders/default/vertex.glsl", "shaders/default/fragment.glsl", NULL,
        "default");

    Shader light_shader = ResourceManager::LoadShader(
        "shaders/light/vertex.glsl", "shaders/light/fragment.glsl", NULL,
        "light");

    VertexArray        pyramid_va;
    VertexBuffer       pyramid_vb(pyramid_vertices, sizeof(pyramid_vertices));
    IndexBuffer        pyramid_ib(pyramid_indices, 18);
    VertexBufferLayout pyramid_va_layout;
    pyramid_va_layout.push(GL_FLOAT, 3);
    pyramid_va_layout.push(GL_FLOAT, 3);
    pyramid_va_layout.push(GL_FLOAT, 2);
    pyramid_va_layout.push(GL_FLOAT, 3);
    pyramid_va.add_buffer(pyramid_vb, pyramid_va_layout);

    VertexArray        light_va;
    VertexBuffer       light_vb(light_vertices, sizeof(light_vertices));
    IndexBuffer        light_ib(light_indices, 36);
    VertexBufferLayout light_va_layout;
    light_va_layout.push(GL_FLOAT, 3);
    light_va.add_buffer(light_vb, light_va_layout);

    glm::mat4 projection;
    projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::scale(model, glm::vec3(1.0f));

    glm::mat4 model2 = glm::mat4(1.0f);
    model2           = glm::translate(model2, glm::vec3(-4.0f, 4.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where
    // we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -0.0f));

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos   = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel           = glm::translate(lightModel, lightPos);

    glm::vec3 pyramidPos   = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 pyramidModel = glm::mat4(1.0f);
    pyramidModel           = glm::translate(pyramidModel, pyramidPos);

    default_shader.setMatrix4("projection", projection);
    default_shader.setMatrix4("model", model);
    default_shader.setMatrix4("view", camera.getViewMatrix());
    default_shader.setVector4f("lightColor", lightColor);

    light_shader.setMatrix4("projection", projection);
    light_shader.setMatrix4("model", lightModel);
    light_shader.setMatrix4("view", camera.getViewMatrix());
    light_shader.setVector4f("lightColor", lightColor);

    while (!glfwWindowShouldClose(window))
    {

        int width  = 0;
        int height = 0;
        glfwGetWindowSize(window, &width, &height);
        projection = glm::perspective(glm::radians(45.0f),
                                      static_cast<float>(width) /
                                          static_cast<float>(height),
                                      0.1f, 100.0f);

        view = camera.getViewMatrix();

        default_shader.setMatrix4("projection", projection);
        default_shader.setMatrix4("view", view);
        default_shader.setVector3f("lightPos", lightPos);
        default_shader.setVector3f("camPos", camera.position);
        brick_texture->bind(0);

        light_shader.setMatrix4("projection", projection);
        light_shader.setMatrix4("view", view);
        // calculate delta time
        // --------------------
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

        // manage user input
        // -----------------
        game.ProcessInput(deltaTime);

        // update game state
        // -----------------
        game.Update(deltaTime);

        // render
        // ------
        RenderAPI::clear();

        renderer.Draw(light_vb, light_va,
                      sizeof(light_indices) / sizeof(unsigned int),
                      light_shader);

        renderer.Draw(pyramid_vb, pyramid_va,
                      sizeof(pyramid_indices) / sizeof(unsigned int),
                      default_shader);

        // default_shader.setMatrix4("model", model2);
        // renderer.Draw(light_vb, light_va, 36, default_shader);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Resources");
            ImGui::SeparatorText("Textures");

            for (auto tex : ResourceManager::textures)
            {
                ImGui::Text(tex.first.c_str());
                auto texture = tex.second;
                ImGui::Image((ImTextureID)(intptr_t)tex.second->getId(),
                             ImVec2(100, 100));
            }

            ImGui::SeparatorText("Shaders");

            for (auto shad : ResourceManager::shaders)
            {
                ImGui::Text("%s", shad.first.c_str());
                auto shader = shad.second;
            }

            ImGui::End();
        }

        game.Render();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            ImGui::Begin("Hello, world!");
            ImGui::SetWindowPos(
                {static_cast<float>(game.Width) - 400.0f, 0.0f});
            ImGui::SetWindowSize({400.0f, static_cast<float>(game.Height)});
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);

            ImGui::Value("Mouse X", game.mouse_pos.x);
            ImGui::SameLine();
            ImGui::Value("Mouse Y", game.mouse_pos.y);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        render_context.swapBuffers();
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
                  int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property
    // to true, closing the application
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
    {
        game.Keys[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        game.Keys[button] = false;
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    RenderAPI::updateViewPort(width, height);
    game.UpdateScreenSize(width, height);
}
