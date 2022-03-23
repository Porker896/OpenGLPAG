#pragma once
#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include "LightObject.h"

class LightManager
{
	std::list<LightObject*> lights;

	Shader* lightShader = nullptr;

public:
	void Update();
	void AddLight(LightObject* light);

};

#endif