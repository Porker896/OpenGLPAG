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
	virtual void Update();
	void AddChild(Object* child);
	void Draw();
};

#endif