#include "resource_manager.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <string>

#include "stb_image.h"
#include "texture.h"

// Instantiate static variables
std::unordered_map<std::string, Texture2D> ResourceManager::Textures;
std::unordered_map<std::string, Shader>    ResourceManager::shaders;

Shader ResourceManager::LoadShader(const char* vShaderFile,
                                   const char* fShaderFile,
                                   const char* gShaderFile, std::string name)
{
    shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha,
                                       std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    const auto it = Textures.find(name);
    if (it == Textures.end())
    {
        printf("ResourceManager: Can't find texture %s", name.c_str());
    }
    return Textures[name];
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders
    for (auto iter : shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile,
                                           const char* fShaderFile,
                                           const char* gShaderFile)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // open files
        std::ifstream     vertexShaderFile(vShaderFile);
        std::ifstream     fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream     geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode,
                   gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format    = GL_RGBA;
    }
    // load image
    int            width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cout << "Cant open texture: " << file << std::endl;
    }
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}
