#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "Model.h"
#include "Transform.h"
#include <list>

class Object
{

	std::list<Object*> children;

	Model* model = nullptr;

	Shader* shader = nullptr;


public:

	Object* parent = nullptr;
	Transform* transform = nullptr;

	Object(const std::string& modelPath, Shader* objShader);
	~Object();
	void Update();
	void AddChild(Object* child);
	void Draw() const;
};

#endif