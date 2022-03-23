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
	shader->use();
	shader->setMat4("model", transform->getLocalModelMatrix());
	model->Draw(*shader);
}
