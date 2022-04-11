#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <vector>


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model 
{
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const &path, bool gamma = false, const glm::mat4* instanceMatrices = nullptr, const unsigned amount = 1);

    // draws the model, and thus all its meshes
    void Draw(Shader &shader);

    void DrawInstanced(Shader& shader, const unsigned int amount);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel(std::string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void ProcessNode(aiNode *node, const aiScene *scene);

    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif