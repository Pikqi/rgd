# cmake_minimum_required(VERSION 3.10)

cmake_minimum_required(VERSION 3.15)
project(rgd VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Gather all .cpp and .c files in src/
file(GLOB SOURCES "src/*.cpp" "src/*.c")

set(THIRD_PARTY_SOURCES "include/glad/glad.c")

file(GLOB IMGUI_SOURCES
    "include/imgui/*.cpp"
    "include/imgui/backends/imgui_impl_glfw.cpp"
    "include/imgui/backends/imgui_impl_opengl3.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${THIRD_PARTY_SOURCES} ${IMGUI_SOURCES} )

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC include src include/imgui include/imgui/backends)

# --- Link OpenGL ---
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# --- Find and link GLFW ---
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARIES})

find_package(spdlog REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)

# --- Run step --- 
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_dependencies(run ${PROJECT_NAME})

# --- Debug step --- 
add_custom_target(debug
    COMMAND gdb ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS ${PROJECT_NAME}
)
