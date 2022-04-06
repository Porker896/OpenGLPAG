#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "Model.h"
#include "Transform.h"

class Object
{
protected:

	bool instanced = false;

	std::vector<Object*> children;

	Model* model = nullptr;

	Shader* shader = nullptr;


public:

	Object* parent = nullptr;

	Transform* transform = nullptr;

	Object(const std::string& modelPath, Shader* objShader);

	Object();

	Object(Model* loadedModel, Shader* objShader);

	virtual ~Object() = default;

	void SetModel(Model* newModel);

	void SetShader(Shader* newShader);

	void SetTransform(Transform* newTransform); 

	void AddChild(Object* child);

	virtual void Update();

	virtual void Draw();
};

class InstancedObject : public Object
{
	unsigned int instanceMatBuffer = 0;

	std::vector<Transform> instanceTransforms;

	void PrepareInstanceMatricesBuffer();

	void UpdateInstanceMatricesBuffer();

public:
	InstancedObject();

	~InstancedObject() override = default;

	InstancedObject(Model* objModel, Shader* objShader, const std::vector<Transform>& objInstanceTransforms);

	void Update() override;

	void Draw() override;

	void AddInstanceTransform(const Transform& transform);


};

#endif