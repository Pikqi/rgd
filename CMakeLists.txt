# cmake_minimum_required(VERSION 3.10)

cmake_minimum_required(VERSION 3.15)
project(rgd VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all .cpp and .c files in src/
file(GLOB SOURCES "src/*.cpp" "src/*.c")

file(GLOB IMGUI_SOURCES
    "src/imgui/*.cpp"
    "src/imgui/backends/imgui_impl_glfw.cpp"
    "src/imgui/backends/imgui_impl_opengl3.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES} )

# Include directories (GLAD and your headers)
target_include_directories(${PROJECT_NAME} PUBLIC include src src/imgui src/imgui/backends)

# --- Link OpenGL ---
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# --- Find and link GLFW ---
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES})

# --- Run step --- 
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_dependencies(run ${PROJECT_NAME})
