#pragma once
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
  public:
    Shader() {}
    void use();
    void compile(const char* vertexSource, const char* fragmentSource,
                 const char* geometrySource = nullptr);
    // utility functions
    void setFloat(const char* name, float value);
    void setInteger(const char* name, int value);
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

  private:
    void checkCompileErrors(unsigned int object, std::string type);
    int  getUniformLocation(const char* name) const;

    unsigned int _id;
};
