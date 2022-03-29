#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

class Transform
{
	glm::vec3 pos = {0,0,0};
	glm::vec3 eulerRot = {0,0,0};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	bool dirty = true;

public:
	Transform();

	void computeModelMatrix();
	void computeModelMatrix(const glm::mat4& parentGlobalMatrix);
	void setLocalRotation(const glm::vec3& newRotation);

	void setLocalPosition(const glm::vec3& newPosition);
	void setLocalRotationX(const float newX);
	void setLocalRotationY(const float newY);
	void setLocalRotationZ(const float newZ);
	void setModelMatrix(const glm::mat4& newModel);
	void setLocalScale(const glm::vec3& newScale);

	const glm::vec3& getLocalPosition() const;

	const glm::mat4& getLocalModelMatrix() const;

	bool isDirty() const;


};


#endif
