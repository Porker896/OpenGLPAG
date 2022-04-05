#include "LightObject.h"

void SpotLight::PassToShader(Shader* shader, const int id) const
{
	std::string name("spotLights[");
	name.append(std::to_string(id) + "]");
	const auto buff = name;

	shader->setBool(name.append(".isActive"), isActive);
	name = buff;

	shader->setVec3(name.append(".colors.ambient"), colors.ambient);
	name = buff;

	shader->setVec3(name.append(".colors.diffuse"), colors.diffuse);
	name = buff;

	shader->setVec3(name.append(".colors.specular"), colors.specular);
	name = buff;

	shader->setFloat(name.append(".cutOff"), cutOff);
	name = buff;

	shader->setFloat(name.append(".outerCutOff"), outerCutOff);
	name = buff;

	shader->setFloat(name.append(".att.constant"), att.constant);
	name = buff;

	shader->setFloat(name.append(".att.linear"), att.linear);
	name = buff;

	shader->setFloat(name.append(".att.quadratic"), att.quadratic);
	name = buff;


}

void PointLight::PassToShader(Shader* shader, const int id) const
{
	std::string name = "pointLights[";
	name.append(std::to_string(id) + "]");
	const std::string buff = name;

	shader->setBool(name.append(".isActive"), isActive);
	name = buff;
	shader->setVec3(name.append(".colors.ambient"), colors.ambient);
	name = buff;

	shader->setVec3(name.append(".colors.diffuse"), colors.diffuse);
	name = buff;

	shader->setVec3(name.append(".colors.specular"), colors.specular);
	name = buff;

	shader->setFloat(name.append(".att.constant"), att.constant);
	name = buff;

	shader->setFloat(name.append(".att.linear"), att.linear);
	name = buff;

	shader->setFloat(name.append(".att.quadratic"), att.quadratic);
	name = buff;

}

LightObject::LightObject(const std::string& path, Shader* objShader, Shader* sceneLightShader) : Object(path, objShader)
{
	lightShader = sceneLightShader;
	id += lightObjectId;
	lightObjectId++;
}

LightObject::LightObject(const std::string& path, Shader* objShader) : Object(path, objShader)
{
	id += lightObjectId;
	lightObjectId++;
}

void LightObject::PassToShader() 
{
	pointLight.position = transform->GetLocalPosition();

	spotLight.PassToShader(lightShader, id);
	pointLight.PassToShader(lightShader, id);
}
