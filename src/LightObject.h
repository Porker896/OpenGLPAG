#pragma once
#ifndef LIGHT_OBJECT_H
#define LIGHT_OBJECT_H

#include "Object.h"

struct BaseLight
{
	glm::vec3 ambient = glm::vec3(0.0f);
	glm::vec3 diffuse = glm::vec3(0.0f);
	glm::vec3 specular = glm::vec3(0.0f);
};

struct Attenuation
{
	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 1.8f;
};

struct PointLight
{
	bool isActive = false;
	glm::vec3 position = {0.0f,0.0f,0.0f};
	BaseLight colors;
	Attenuation att;
	void PassToShader(Shader* shader, const int id) const;
};

struct SpotLight : PointLight
{

	glm::vec3 direction = { 0.0f, 0.0f, -1.0f };

	float cutOff = 0.91f;
	float outerCutOff = 0.82f;

	void PassToShader(Shader* shader, const int id) const;
};

class LightObject : public Object
{
	inline static int lightObjectId = 1;

	inline static Shader* lightShader = nullptr;

public:
	int id = -1;

	SpotLight spotLight;

	PointLight pointLight;

	LightObject(const std::string& path, Shader* shader, Shader* lightShader);

	LightObject(const std::string& path, Shader* objShader);

	void PassToShader();
};


#endif