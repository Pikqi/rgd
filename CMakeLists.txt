project(rgd VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendor/spdlog)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC vendor/imgui vendor/imgui/backends)
target_link_libraries(imgui PRIVATE glfw)

set(SOURCES
    src/camera3d.cpp
    src/clouds.cpp
    src/debug.cpp
    src/framebuffer.cpp
    src/game.cpp
    src/index_buffer.cpp
    src/main.cpp
    src/mesh.cpp
    src/post_process_chain.cpp
    src/post_process_effect.cpp
    src/primitives.cpp
    src/render_api.cpp
    src/render_context.cpp
    src/renderer.cpp
    src/resource_manager.cpp
    src/shader.cpp
    src/sky.cpp
    src/terrain.cpp
    src/terrain_generator.cpp
    src/texture.cpp
    src/vertex_array.cpp
    src/vertex_buffer.cpp
    src/water.cpp
    vendor/stb/stb_image.cpp
    vendor/stb/stb_perlin.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} vendor/glad/glad.c)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE vendor vendor/stb src)

find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw imgui spdlog::spdlog)

# --- Run step ---
add_custom_target(run
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${PROJECT_NAME}
)

# --- Debug step ---
if(UNIX)
    add_custom_target(debug
        COMMAND gdb $<TARGET_FILE:${PROJECT_NAME}>
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        DEPENDS ${PROJECT_NAME}
    )
endif()
