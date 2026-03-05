#include "shader.h"
#include "logger.h"
#include "rgd.h"

void Shader::use() const
{
    GLCALL(glUseProgram(this->_id));
}

void Shader::compile(const char* vertexSource, const char* fragmentSource,
                     const char* geometrySource)
{
    unsigned int sVertex, sFragment, gShader;
    // vertex Shader
    GLCALL(sVertex = glCreateShader(GL_VERTEX_SHADER));
    GLCALL(glShaderSource(sVertex, 1, &vertexSource, NULL));
    GLCALL(glCompileShader(sVertex));
    checkCompileErrors(sVertex, "VERTEX");
    // fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    GLCALL(glShaderSource(sFragment, 1, &fragmentSource, NULL));
    GLCALL(glCompileShader(sFragment));
    checkCompileErrors(sFragment, "FRAGMENT");
    // if geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        GLCALL(glShaderSource(gShader, 1, &geometrySource, NULL));
        GLCALL(glCompileShader(gShader));
        checkCompileErrors(gShader, "GEOMETRY");
    }
    // shader program
    GLCALL(this->_id = glCreateProgram());
    GLCALL(glAttachShader(this->_id, sVertex));
    GLCALL(glAttachShader(this->_id, sFragment));
    if (geometrySource != nullptr)
    {
        GLCALL(glAttachShader(this->_id, gShader));
    }
    GLCALL(glLinkProgram(this->_id));
    checkCompileErrors(this->_id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer
    // necessary
    GLCALL(glDeleteShader(sVertex));
    GLCALL(glDeleteShader(sFragment));
    if (geometrySource != nullptr)
    {
        GLCALL(glDeleteShader(gShader));
    }
}

void Shader::setFloat(const char* name, float value)
{
    this->use();
    GLCALL(glUniform1f(getUniformLocation(name), value));
}
void Shader::setInteger(const char* name, int value)
{
    this->use();
    GLCALL(glUniform1i(getUniformLocation(name), value));
}
void Shader::setVector2f(const char* name, float x, float y)
{
    this->use();
    GLCALL(glUniform2f(getUniformLocation(name), x, y));
}
void Shader::setVector2f(const char* name, const glm::vec2& value)
{
    this->use();
    GLCALL(glUniform2f(getUniformLocation(name), value.x, value.y));
}
void Shader::setVector3f(const char* name, float x, float y, float z)
{
    this->use();
    GLCALL(glUniform3f(getUniformLocation(name), x, y, z));
}
void Shader::setVector3f(const char* name, const glm::vec3& value)
{
    this->use();
    GLCALL(glUniform3f(getUniformLocation(name), value.x, value.y, value.z));
}
void Shader::setVector4f(const char* name, float x, float y, float z, float w)
{
    this->use();
    GLCALL(glUniform4f(getUniformLocation(name), x, y, z, w));
}
void Shader::setVector4f(const char* name, const glm::vec4& value)
{
    this->use();
    GLCALL(glUniform4f(getUniformLocation(name), value.x, value.y, value.z,
                       value.w));
}
void Shader::setMatrix4(const char* name, const glm::mat4& matrix)
{
    this->use();
    GLCALL(glUniformMatrix4fv(getUniformLocation(name), 1, false,
                              glm::value_ptr(matrix)));
}

int Shader::getUniformLocation(const char* name) const
{

    GLCALL(int location = glGetUniformLocation(this->_id, name));
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

        GLCALL(glGetShaderiv(object, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            GLCALL(glGetShaderInfoLog(object, 1024, NULL, infoLog));
            LOG_FATAL(
                "SHADER: Compile-time error: Type: {}\n"
                "{}"
                "\n -- --------------------------------------------------- ",
                type, infoLog);
        }
    }
    else
    {
        GLCALL(glGetProgramiv(object, GL_LINK_STATUS, &success));
        if (!success)
        {
            GLCALL(glGetProgramInfoLog(object, 1024, NULL, infoLog));
            LOG_FATAL(
                "Shader: Link-time error: Type: {}\n"
                "{}"
                "\n -- --------------------------------------------------- "
                "-- ");
        }
    }
}
