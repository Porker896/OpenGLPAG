#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vector>
#include <glm/glm.hpp>

class Transform
{
	bool dirty = true;

	glm::vec3 pos = { 0,0,0 };
	glm::vec3 eulerRot = { 0,0,0 };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 modelMatrix = glm::mat4(1.0f);


public:
	Transform();
	Transform(const glm::mat4& model);

	void ComputeModelMatrix();
	void ComputeModelMatrix(const glm::mat4& parentGlobalMatrix);

	void SetLocalRotation(const glm::vec3& newRotation);
	void SetLocalPosition(const glm::vec3& newPosition);
	void SetLocalRotationX(const float newX);
	void SetLocalRotationY(const float newY);
	void SetLocalRotationZ(const float newZ);
	void SetLocalScale(const glm::vec3& newScale);
	void SetModelMatrix(const glm::mat4& newModel);

	const glm::vec3& GetLocalPosition() const;

	const glm::mat4& GetModelMatrix() const;

	bool isDirty() const;


};




#endif
