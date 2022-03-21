#include "LightObject.h"

void SpotLight::PassToShader(Shader* shader) const
{
	shader->setBool("spotLight.isActive", isActive);
	shader->setVec3("spotLight.ambient", ambient);
	shader->setVec3("spotLight.diffuse", diffuse);
	shader->setVec3("spotLight.specular", specular);

	shader->setFloat("spotLight.cutOff", cutOff);
	shader->setFloat("spotLight.outerutOff", outerCutOff);
	shader->setFloat("spotLight.constant", constant);
	shader->setFloat("spotLight.linear", linear);
	shader->setFloat("spotLight.quadratic", quadratic);

}

void PointLight::PassToShader(Shader* shader) const
{
		shader->setBool("pointLight.isActive", isActive);
	shader->setVec3("pointLight.ambient", ambient);
	shader->setVec3("pointLight.diffuse", diffuse);
	shader->setVec3("pointLight.specular", specular);

	shader->setFloat("pointLight.constant", constant);
	shader->setFloat("pointLight.linear", linear);
	shader->setFloat("pointLight.quadratic", quadratic);
}

void LightObject::passToShader()
{
	spotLight.PassToShader(shader);
	pointLight.PassToShader(shader);
}
