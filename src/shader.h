#pragma once
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
  public:
    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;

    Shader() {}
    void use() const;
    void compile(const char* vertexSource, const char* fragmentSource);
    void reCompile();
    void replace(const int id);

    // utility functions
    void setFloat(const char* name, float value);
    void setInteger(const char* name, int value);
    void setBool(const char* name, bool value);
    void setVector2f(const char* name, float x, float y);
    void setVector2f(const char* name, const glm::vec2& value);
    void setVector3f(const char* name, float x, float y, float z);
    void setVector3f(const char* name, const glm::vec3& value);
    void setVector4f(const char* name, float x, float y, float z, float w);
    void setVector4f(const char* name, const glm::vec4& value);
    void setMatrix4(const char* name, const glm::mat4& matrix);

    unsigned int get_id()
    {
        return _id;
    }

    bool has_error()
    {
        return _error;
    };
    void clear_error()
    {
        _error = false;
    }

  private:
    bool checkCompileErrors(unsigned int object, std::string type);
    int  getUniformLocation(const char* name) const;

    unsigned int _id;
    bool         _error;
};
