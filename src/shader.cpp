#include "shader.h"
#include "logger.h"

void Shader::use()
{
    glUseProgram(this->_id);
}

void Shader::compile(const char* vertexSource, const char* fragmentSource,
                     const char* geometrySource)
{
    unsigned int sVertex, sFragment, gShader;
    // vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    // if geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
    }
    // shader program
    this->_id = glCreateProgram();
    glAttachShader(this->_id, sVertex);
    glAttachShader(this->_id, sFragment);
    if (geometrySource != nullptr)
        glAttachShader(this->_id, gShader);
    glLinkProgram(this->_id);
    checkCompileErrors(this->_id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer
    // necessary
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
}

void Shader::setFloat(const char* name, float value)
{
    this->use();
    glUniform1f(getUniformLocation(name), value);
}
void Shader::setInteger(const char* name, int value)
{
    this->use();
    glUniform1i(getUniformLocation(name), value);
}
void Shader::setVector2f(const char* name, float x, float y)
{
    this->use();
    glUniform2f(getUniformLocation(name), x, y);
}
void Shader::setVector2f(const char* name, const glm::vec2& value)
{
    this->use();
    glUniform2f(getUniformLocation(name), value.x, value.y);
}
void Shader::setVector3f(const char* name, float x, float y, float z)
{
    this->use();
    glUniform3f(getUniformLocation(name), x, y, z);
}
void Shader::setVector3f(const char* name, const glm::vec3& value)
{
    this->use();
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}
void Shader::setVector4f(const char* name, float x, float y, float z, float w)
{
    this->use();
    glUniform4f(getUniformLocation(name), x, y, z, w);
}
void Shader::setVector4f(const char* name, const glm::vec4& value)
{
    this->use();
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix4(const char* name, const glm::mat4& matrix)
{
    this->use();
    glUniformMatrix4fv(getUniformLocation(name), 1, false,
                       glm::value_ptr(matrix));
}

int Shader::getUniformLocation(const char* name) const
{

    int location = glGetUniformLocation(this->_id, name);
    if (location == -1)
    {
        LOG_WARN(
            "SHADER: Uniform with name {} does not exist, on shader id: {}",
            name, _id);
    }
    return location;
};

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int  success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            LOG_FATAL(
                "SHADER: Compile-time error: Type: {}\n"
                "{}"
                "\n -- --------------------------------------------------- ",
                type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            LOG_FATAL(
                "Shader: Link-time error: Type: {}\n"
                "{}"
                "\n -- --------------------------------------------------- "
                "-- ");
        }
    }
}
