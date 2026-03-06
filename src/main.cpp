#include "game.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>
#include "glm/ext/matrix_clip_space.hpp"
#include "index_buffer.h"
#include "resource_manager.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
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

bool show_demo_window = false;
int  main(int argc, char* argv[])
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

    const float vertices[] = {
        // Front face
        -1.0f, -1.0f, 1.0f, 0.5f, // 0: bottom-left
        1.0f, -1.0f, 1.0f, 0.5f,  // 1: bottom-right
        1.0f, 1.0f, 1.0f, 0.5f,   // 2: top-right
        -1.0f, 1.0f, 1.0f, 0.5f,  // 3: top-left

        // Back face
        -1.0f, -1.0f, -1.0f, 1.0f, // 4: bottom-left
        1.0f, -1.0f, -1.0f, 1.0f,  // 5: bottom-right
        1.0f, 1.0f, -1.0f, 1.0f,   // 6: top-right
        -1.0f, 1.0f, -1.0f, 1.0f   // 7: top-left
    };

    const unsigned int indices[]    = {// Front face
                                    0, 1, 2, 2, 3, 0,
                                    // Right face
                                    1, 5, 6, 6, 2, 1,
                                    // Back face
                                    5, 4, 7, 7, 6, 5,
                                    // Left face
                                    4, 0, 3, 3, 7, 4,
                                    // Bottom face
                                    4, 5, 1, 1, 0, 4,
                                    // Top face
                                    3, 2, 6, 6, 7, 3};
    Shader             basic_shader = ResourceManager::LoadShader(
        "shaders/basic_textured/vertex.glsl",
        "shaders/basic_textured/fragment.glsl", NULL, "basic");

    VertexArray        va;
    VertexBuffer       vb(vertices, sizeof(vertices));
    IndexBuffer        ib(indices, 36);
    VertexBufferLayout va_layout;
    va_layout.push(GL_FLOAT, 3);
    va_layout.push(GL_FLOAT, 1);
    va.add_buffer(vb, va_layout);

    texture->bind();
    // basic_shader.setInteger("texture1", 0);

    glm::mat4 projection;
    projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model =
        glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where
    // we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));

    basic_shader.setMatrix4("projection", projection);
    basic_shader.setMatrix4("model", model);
    basic_shader.setMatrix4("view", view);

    va.unbind();
    vb.unbind();
    ib.unbind();
    while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime          = currentFrame - lastFrame;
        lastFrame          = currentFrame;
        glfwPollEvents();

        // manage user input
        // -----------------
        game.ProcessInput(deltaTime);

        // update game state
        // -----------------
        game.Update(deltaTime);

        // model = glm::rotate(model, glm::radians(-55.0f + deltaTime * 0.001f),
        //                     glm::vec3(1.0f, 0.0f, 0.0f));

        basic_shader.setMatrix4("model", model);

        // render
        // ------
        RenderAPI::clear();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        game.Render();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        if (game.debug_mode)
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

        renderer.Draw(vb, va, 36, basic_shader);

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
