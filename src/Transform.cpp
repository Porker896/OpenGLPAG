#include "Transform.h"

#include "glm/trigonometric.hpp"
#include "glm/ext/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

Transform::Transform() = default;

Transform::Transform(const glm::mat4 & model) : modelMatrix(model)
{
	glm::quat rot;
	glm::vec4 perspective;
	glm::vec3 skew;
	glm::decompose(model, scale, rot, pos, skew, perspective);
	eulerRot = glm::eulerAngles(rot);

}

void Transform::Update(bool parentDirty)
{
	parentDirty |= dirty;
	if (parentDirty)
	{
		ComputeModelMatrix();
	}

	for (const auto& child : children)
	{
		child->Update(parentDirty);
	}

}

void Transform::ComputeModelMatrix()
{
	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
		glm::radians(eulerRot.x),
		glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
		glm::radians(eulerRot.y),
		glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
		glm::radians(eulerRot.z),
		glm::vec3(0.0f, 0.0f, 1.0f));

	const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

	modelMatrix = glm::translate(glm::mat4(1.0f), pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);

	if(parent != nullptr)
		modelMatrix = parent->GetModelMatrix() * modelMatrix;

	dirty = false;
}

void Transform::ComputeModelMatrix(const glm::mat4 & parentGlobalMatrix)
{
	ComputeModelMatrix();

	modelMatrix = parentGlobalMatrix * modelMatrix;
}

void Transform::SetParent(Transform * parent)
{
	this->parent = parent;
	parent->AddChild(this);
}

void Transform::AddChild(Transform * child)
{
	children.emplace_back(child);
}

void Transform::SetLocalRotation(const glm::vec3 & newRotation)
{
	eulerRot = newRotation;
	dirty = true;
}

void Transform::SetLocalPosition(const glm::vec3 & newPosition)
{
	pos = newPosition;
	dirty = true;
}

void Transform::SetLocalRotationX(const float newX)
{
	pos.x = newX;
	dirty = true;
}

void Transform::SetLocalRotationY(const float newY)
{
	pos.y = newY;
	dirty = true;
}

void Transform::SetLocalRotationZ(const float newZ)
{
	pos.z = newZ;
	dirty = true;
}

void Transform::SetModelMatrix(const glm::mat4 & newModel)
{
	modelMatrix = newModel;
	glm::quat rot;
	glm::vec4 perspective;
	glm::vec3 skew;

	glm::decompose(modelMatrix, scale, rot, pos, skew, perspective);
	eulerRot = glm::eulerAngles(rot);
	dirty = true;
}

void Transform::SetLocalScale(const glm::vec3 & newScale)
{
	scale = newScale;
	dirty = true;
}


const glm::vec3& Transform::GetLocalPosition() const
{
	return pos;
}

const glm::vec3& Transform::GetLocalRotation() const
{
	return eulerRot;
}

const glm::vec3& Transform::GetLocalScale() const
{
	return scale;
}

const glm::mat4& Transform::GetModelMatrix() const
{
	return modelMatrix;
}

bool Transform::isDirty() const
{
	return dirty;
}

