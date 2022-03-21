#pragma once
#ifndef LIGHT_OBJECT_H
#define LIGHT_OBJECT_H

#include "Object.h"

struct PointLight
{
	bool isActive = false;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant, linear, quadratic;

	void PassToShader(Shader* shader) const;

};

struct SpotLight
{
	bool isActive;

	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void PassToShader(Shader* shader) const;
};

class LightObject : public Object
{
	SpotLight spotLight;
	PointLight pointLight;

public:
	void passToShader();
};


#endif