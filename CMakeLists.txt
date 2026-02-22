# cmake_minimum_required(VERSION 3.10)

include(FetchContent)

cmake_minimum_required(VERSION 3.15)
project(rgd VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v6.0.4 
)

FetchContent_MakeAvailable(assimp)

# Gather all .cpp and .c files in src/
file(GLOB SOURCES "src/*.cpp" "src/*.c")

file(GLOB IMGUI_SOURCES
    "include/imgui/*.cpp"
    "include/imgui/backends/imgui_impl_glfw.cpp"
    "include/imgui/backends/imgui_impl_opengl3.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} )

# Include directories (GLAD and your headers)
target_include_directories(${PROJECT_NAME} PUBLIC include src include/imgui include/imgui/backends)

# --- Link OpenGL ---
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# --- Find and link GLFW ---
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES} assimp)

# --- Run step --- 
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_dependencies(run ${PROJECT_NAME})


