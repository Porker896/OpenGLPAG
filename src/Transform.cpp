#include "Transform.h"

#include "glm/trigonometric.hpp"
#include "glm/ext/matrix_transform.hpp"

Transform::Transform() = default;


void Transform::computeModelMatrix()
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

	dirty = false;
}

void Transform::computeModelMatrix(const glm::mat4& parentGlobalMatrix)
{
	computeModelMatrix();
	modelMatrix = parentGlobalMatrix * modelMatrix;
}

void Transform::setLocalRotation(const glm::vec3 & newRotation)
{
	eulerRot = newRotation;
	dirty = true;
}

void Transform::setLocalPosition(const glm::vec3 & newPosition)
{
	pos = newPosition;
	dirty = true;
}

void Transform::setLocalRotationX(const float newX)
{
	pos.x = newX;
	dirty = true;
}

void Transform::setLocalRotationY(const float newY)
{
	pos.y = newY;
	dirty = true;
}

void Transform::setLocalRotationZ(const float newZ)
{
	pos.z = newZ;
	dirty = true;
}

void Transform::setLocalScale(const glm::vec3 & newScale)
{
	scale = newScale;
	dirty = true;
}

const glm::vec3& Transform::getLocalPosition() const
{
	return pos;
}

const glm::mat4& Transform::getLocalModelMatrix() const
{
	return modelMatrix;
}

bool Transform::isDirty()
{
	return dirty;
}

