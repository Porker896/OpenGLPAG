#pragma once

#ifndef MESH_H
#define MESH_H


#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "Shader.h"

#define MAX_BONE_INFLUENCE 4


struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, const glm::mat4* instanceMatrices = nullptr,
        const unsigned instanceCount = 1);
    // render the mesh
    void Draw(const Shader &shader);

    void DrawInstanced(Shader& shader, const unsigned int amount);

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh(const glm::mat4* instanceMatrices = nullptr, const unsigned int amount = 1);

    void setupInstancedMesh(const glm::mat4* instanceMatrices, const unsigned int amount);
   
};
#endif