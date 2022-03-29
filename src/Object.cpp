#include "Object.h"

Object::Object(const std::string& modelPath, Shader* objShader) : model(new Model(modelPath)), shader(objShader), transform(new Transform())
{

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
		transform->computeModelMatrix(parent->transform->getLocalModelMatrix());
	}
	else
	{
		transform->computeModelMatrix();
	}

	if (!children.empty())
	{
		for (auto&& child : children)
			child->Update();
	}
}

void Object::Draw()
{
	if (model != nullptr)
	{
		shader->use();
		shader->setMat4("model", transform->getLocalModelMatrix());
		model->Draw(*shader);
	}
	
	for(auto& child : children)
	{
		child->Draw();
	}
}

InstancedObject::InstancedObject(const std::string& modelPath, Shader* objShader, glm::mat4* instanceMatrices,
	unsigned instanceAmount) : Object(modelPath, objShader), instanceMatrices(instanceMatrices), amount(instanceAmount)
{
	for (const auto& mesh : model->meshes)
	{
		glBindVertexArray(mesh.VAO);

		unsigned int matrixBuffer = 0;

		glGenBuffers(1, &matrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);

		// vertex attributes
		const std::size_t vec4Size = sizeof(glm::vec4);
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

void InstancedObject::Update()
{
	Object::Update();
}

void InstancedObject::Draw()
{
	shader->use();

	model->DrawInstanced(*shader, amount);

	for(auto& child : children)
	{
		child->Draw();
	}
}
