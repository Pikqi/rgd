#include "resource_manager.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <fstream>
#include <string>

#include "logger.h"
#include "stb_image.h"
#include "texture.h"

// Instantiate static variables
std::unordered_map<std::string, std::shared_ptr<Texture>>
                                        ResourceManager::textures;
std::unordered_map<std::string, Shader> ResourceManager::shaders;

Shader& ResourceManager::LoadShader(const char* vShaderFile,
                                    const char* fShaderFile, std::string name)
{
    shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile);
    return shaders[name];
}

void ResourceManager::ReCompileShaders()
{

    for (auto& [name, shader] : shaders)
    {

        Shader recompiled = loadShaderFromFile(shader.vertexPath.c_str(),
                                               shader.fragmentPath.c_str());
        if (!recompiled.has_error())
        {
            shader.replace(recompiled.get_id());
        }
    }
    LOG_INFO("Shaders recompiled");
}

Shader& ResourceManager::GetShader(std::string name)
{
    return shaders[name];
}

std::shared_ptr<Texture>
ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    textures[name] = loadTextureFromFile(file, alpha);
    return textures[name];
}

std::shared_ptr<Texture> ResourceManager::GetTexture(std::string name)
{
    const auto it = textures.find(name);
    if (it == textures.end())
    {
        printf("ResourceManager: Can't find texture %s", name.c_str());
    }
    // todo: add fallback texture
    return textures[name];
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders
    for (auto iter : shaders)
        glDeleteProgram(iter.second.get_id());
    textures.clear();
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile,
                                           const char* fShaderFile)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
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
    } catch (std::exception e)
    {
        LOG_FATAL("ERROR::SHADER: Failed to read shader files {} {}",
                  vShaderFile, fShaderFile);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.compile(vShaderCode, fShaderCode);
    shader.vertexPath   = std::string(vShaderFile);
    shader.fragmentPath = std::string(fShaderFile);
    return shader;
}

std::shared_ptr<Texture> ResourceManager::loadTextureFromFile(const char* file,
                                                              bool        alpha)
{
    int            width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data)
    {
        LOG_WARN("Cant open texture: {}", file);
    }
    auto texture =
        std::make_unique<Texture>(data, width, height, alpha, nrChannels);

    stbi_image_free(data);
    return texture;
}
