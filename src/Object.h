#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "Model.h"
#include "Transform.h"

class Object
{
protected:

	Model* model = nullptr;

	Shader* shader = nullptr;

public:

	Transform transform;

	Object(const std::string& modelPath, Shader* objShader);

	Object(Model* loadedModel, Shader* objShader);

	Object();
	
	virtual ~Object() = default;

	void SetModel(Model* newModel);

	void SetShader(Shader* newShader);

	virtual void Update();

	virtual void Draw();
};

class InstancedObject : public Object
{
	unsigned int instanceMatBuffer = 0;


	void PrepareInstanceMatricesBuffer();

	void UpdateInstanceMatricesBuffer();

public:
	InstancedObject();

	~InstancedObject() override = default;

	InstancedObject(Model* objModel, Shader* objShader, std::vector<Transform*> objInstanceTransforms);

	void Update() override;

	void Draw() override;

	void AddInstanceTransform(const Transform& transform);

	std::vector<Transform*> instanceTransforms;


};

#endif