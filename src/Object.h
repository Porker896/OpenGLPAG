#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "Model.h"
#include "Transform.h"
#include <list>

class Object
{
protected:

	std::list<Object*> children;

	Model* model = nullptr;

	Shader* shader = nullptr;


public:

	Object* parent = nullptr;

	Transform* transform = nullptr;

	Object(const std::string& modelPath, Shader* objShader);

	Object() = default;

	virtual void Update();

	void AddChild(Object* child);

	virtual void Draw();
};

class InstancedObject : public Object
{
	glm::mat4* instanceMatrices = nullptr;
	unsigned int amount = 1;
public:
	InstancedObject(const std::string& modelPath, Shader* objShader, glm::mat4* instanceMatrices, unsigned int instanceAmount);

	void Update() override;

	void Draw() override;

	
};

#endif