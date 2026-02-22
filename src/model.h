#pragma once

#include "assimp/scene.h"
#include "mesh.h"
#include "shader.h"

class Model
{
  public:
    Model(char* path);
    void Draw(Shader& shader);

  private:
    // model data
    std::vector<Mesh> meshes;
    std::string       directory;

    void                 loadModel(std::string path);
    void                 processNode(aiNode* node, const aiScene* scene);
    Mesh                 processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial*   mat,
                                              aiTextureType type,
                                              std::string   typeName);
};
