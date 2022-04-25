#include "Object.h"

#include "glm/gtc/type_ptr.hpp"

Object::Object(const std::string& modelPath, Shader* objShader) : model(new Model(modelPath)), shader(objShader)
{

}

Object::Object(Model* loadedModel, Shader* objShader) : model(loadedModel), shader(objShader)
{

}

Object::Object(){};

void Object::SetModel(Model* newModel)
{
	model = newModel;
}

void Object::SetShader(Shader* newShader)
{
	shader = newShader;
}


void Object::Update()
{

}

void Object::Draw()
{
	if (model != nullptr)
	{
		shader->use();
		shader->setMat4("model", transform.GetModelMatrix());
		model->Draw(*shader);
	}

}

InstancedObject::InstancedObject() : Object()
{
	PrepareInstanceMatricesBuffer();
}


InstancedObject::InstancedObject(Model* objModel, Shader* objShader, std::vector<Transform*> objInstanceTransforms) :
	Object(objModel, objShader), instanceTransforms(std::move(objInstanceTransforms))
{
	PrepareInstanceMatricesBuffer();

}

void InstancedObject::Update()
{
	Object::Update();
}

void InstancedObject::Draw()
{
	UpdateInstanceMatricesBuffer();

	if(model != nullptr)
	{
		shader->use();
		model->DrawInstanced(*shader, instanceTransforms.size());
	}
}

void InstancedObject::PrepareInstanceMatricesBuffer()
{
	glGenBuffers(1, &instanceMatBuffer);

	std::vector<glm::mat4> instanceMatrices;
	for (const auto& transform : instanceTransforms)
		instanceMatrices.emplace_back(transform->GetModelMatrix());

	glBindBuffer(GL_ARRAY_BUFFER, instanceMatBuffer);
	glBufferData(GL_ARRAY_BUFFER, static_cast<int>(instanceMatrices.size()) * sizeof(glm::mat4), instanceMatrices.data(), GL_DYNAMIC_DRAW);


	for (const auto& mesh : model->meshes)
	{
		constexpr std::size_t vec4Size = sizeof(glm::vec4);

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
	shader->setMat4("mainObjectModel", transform.GetModelMatrix());

	std::vector<glm::mat4> instanceMatrices;
	for (const auto& transform : instanceTransforms)
		instanceMatrices.emplace_back(transform->GetModelMatrix());

	glBindBuffer(GL_ARRAY_BUFFER, instanceMatBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<int>(instanceMatrices.size()) * sizeof(glm::mat4), instanceMatrices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

