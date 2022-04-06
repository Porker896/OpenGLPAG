#include "Object.h"

#include "glm/gtc/type_ptr.hpp"

Object::Object(const std::string& modelPath, Shader* objShader) : model(new Model(modelPath)), shader(objShader), transform(new Transform())
{

}

Object::Object(Model* loadedModel, Shader* objShader) : model(loadedModel), shader(objShader), transform(new Transform())
{
}

Object::Object() : transform(new Transform())
{

}

void Object::SetModel(Model* newModel)
{
	model = newModel;
}

void Object::SetShader(Shader* newShader)
{
	shader = newShader;
}

void Object::SetTransform(Transform* newTransform)
{
	transform = newTransform;
}


void Object::AddChild(Object* child)
{
	child->parent = this;
	children.emplace_back(child);
}

void Object::Update()
{

	if (!transform->isDirty())
	{
		return;
	}

	if (parent != nullptr)
	{
		transform->ComputeModelMatrix(parent->transform->GetModelMatrix());
	}
	else
	{
		transform->ComputeModelMatrix();
	}

	if (!children.empty())
	{
		for (auto& child : children)
			child->Update();
	}
}

void Object::Draw()
{
	if (model != nullptr)
	{
		shader->use();
		shader->setMat4("model", transform->GetModelMatrix());
		model->Draw(*shader);
	}

	for (const auto& child : children)
	{
		child->Draw();
	}
}

InstancedObject::InstancedObject() : Object()
{
	PrepareInstanceMatricesBuffer();
}


InstancedObject::InstancedObject(Model* objModel, Shader* objShader, const std::vector<Transform>& objInstanceTransforms) :
	Object(objModel, objShader), instanceTransforms(objInstanceTransforms)
{
	PrepareInstanceMatricesBuffer();

}



void InstancedObject::PrepareInstanceMatricesBuffer()
{
	glGenBuffers(1, &instanceMatBuffer);

	std::vector<glm::mat4> instanceMatrices;
	for (const auto& transform : instanceTransforms)
		instanceMatrices.emplace_back(transform.GetModelMatrix());

 	glBindBuffer(GL_ARRAY_BUFFER, instanceMatBuffer);
	glBufferData(GL_ARRAY_BUFFER, static_cast<int>(instanceMatrices.size()) * sizeof(glm::mat4), instanceMatrices.data(), GL_DYNAMIC_DRAW);

	constexpr std::size_t vec4Size = sizeof(glm::vec4);

	for (const auto& mesh : model->meshes)
	{
		glBindVertexArray(mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceMatBuffer);

		// vertex attributes
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);

	}
}

void InstancedObject::UpdateInstanceMatricesBuffer()
{
	std::vector<glm::mat4> instanceMatrices;
	for (const auto& transform : instanceTransforms)
		instanceMatrices.emplace_back(transform.GetModelMatrix());

	glBindBuffer(GL_ARRAY_BUFFER, instanceMatBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<int>(instanceMatrices.size()) * sizeof(glm::mat4), instanceMatrices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedObject::Update()
{
	Object::Update();
	UpdateInstanceMatricesBuffer();
}

void InstancedObject::Draw()
{
	if (model != nullptr)
	{
		shader->use();
		shader->setMat4("model", transform->GetModelMatrix());
		model->DrawInstanced(*shader, instanceTransforms.size());
	}

	for (const auto& child : children)
	{
		child->Draw();
	}
}

void InstancedObject::AddInstanceTransform(const Transform& newTransform)
{
	instanceTransforms.emplace_back(newTransform);
}
