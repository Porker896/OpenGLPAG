#pragma once

#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <vector>
#include <glm/glm.hpp>

class Shader;


struct LightColor
{
	glm::vec3 ambient = glm::vec3(1.0f);
	glm::vec3 diffuse = glm::vec3(1.0f);
	glm::vec3 specular = glm::vec3(1.0f);
};

struct DirLight
{
	bool active = false;
	glm::vec3 dir = glm::vec3(0.0f);
	LightColor color;
};

struct PointLight
{
	bool active = false;
	glm::vec3 pos = glm::vec3(0.0f);
	LightColor color;
	float constant = 1.0f;
	float linear = .7f;
	float quadratic = 1.8f;
};

struct SpotLight : public PointLight
{
	glm::vec3 dir = glm::vec3(0.0f);
	float cutOff = 12.5f;
	float outerCutOff = 17.5f;
};




class LightManager
{
public:

	struct LightState
	{
		glm::mat4 VP = glm::mat4(1.0f);
		glm::vec3 viewPos = glm::vec3(0.0f);
		float shininess = 2.0f;
		glm::vec3 offset = glm::vec3(0.0f);
		int instance = 0;

		DirLight dirLight;

		SpotLight spotLights[2];

		PointLight pointLight;

	} state;

	std::vector<Shader*> shaders;

	LightManager() = default;
	void AddShader(Shader* shader);
	void Update() const;
};


#endif