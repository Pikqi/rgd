
#include "glm/ext/vector_float2.hpp"
#include "resource_manager.h"
#include "shader.h"
#include <glad/glad.h>

static GLuint                 debug_vao;
static GLuint                 debug_vbo;
static Shader                 debug_shader;
static std::vector<glm::vec2> vertices;

void initDebug()
{
    vertices.reserve(64);
    glGenVertexArrays(1, &debug_vao);
    glBindVertexArray(debug_vao);

    glGenBuffers(1, &debug_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glBindVertexArray(0);
    debug_shader = ResourceManager::GetShader("debug");
}

void debugDrawLine(const glm::vec2 a, const glm::vec2 b)
{
    // clang-format off
		vertices.push_back(a);
		vertices.push_back(b);
		//clang-format on
}
void debugDrawRect(const glm::vec2 center, const glm::vec2 size){
	auto wh = size.x / 2.0f;
	auto hh = size.x / 2.0f;
	debugDrawLine({center.x - wh, center.y - hh}, {center.x - wh, center.y + hh});// left
	debugDrawLine({center.x - wh, center.y - hh}, {center.x + wh, center.y - hh});// top
	debugDrawLine({center.x + wh, center.y - hh}, {center.x + wh, center.y + hh});// right
	debugDrawLine({center.x - wh, center.y + hh}, {center.x + wh, center.y + hh});// top
	

}

void debugFlush(){
    debug_shader.use();
    glBindBuffer(GL_ARRAY_BUFFER, debug_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(),
                      GL_STREAM_DRAW);
    glBindVertexArray(debug_vao);
		glDrawArrays(GL_LINES, 0, vertices.size());


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

		vertices.clear();
}
void debugClear(){
	vertices.clear();
}
