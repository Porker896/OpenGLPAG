#include "LightManager.h"
#include "Shader.h"

void LightManager::AddShader(Shader* shader) 
{
	shaders.emplace_back(shader);
}

void LightManager::Update() const 
{
	const float spotLightCutOff[2] =
	{
		glm::cos(glm::radians(state.spotLights[0].cutOff)),
		glm::cos(glm::radians(state.spotLights[1].cutOff))
	};

	const float spotLightOuterCutOff[2] =
	{
		glm::cos(glm::radians(state.spotLights[0].outerCutOff)),
		glm::cos(glm::radians(state.spotLights[1].outerCutOff))
	};

	//update uniforms used in light calculations
	for (const auto& shader : shaders)
	{
		shader->use();
		shader->setMat4("VP", state.VP);
		shader->setVec3("viewPos", state.viewPos);

		shader->setFloat("shininess", state.shininess);
		shader->setVec3("offset", state.offset);
		shader->setInt("chosenInstance", state.instance);

		shader->setBool("dirLight.isActive", state.dirLight.active);
		shader->setVec3("dirLight.direction", state.dirLight.dir);
		shader->setVec3("dirLight.colors.ambient", state.dirLight.color.ambient);
		shader->setVec3("dirLight.colors.diffuse", state.dirLight.color.diffuse);
		shader->setVec3("dirLight.colors.specular", state.dirLight.color.specular);

		//POINT LIGHT
		shader->setBool("pointLights[0].isActive", state.pointLight.active);

		shader->setVec3("pointLights[0].position", state.pointLight.pos);
		shader->setFloat("pointLights[0].att.constant", state.pointLight.constant);
		shader->setFloat("pointLights[0].att.linear", state.pointLight.linear);
		shader->setFloat("pointLights[0].att.quadratic", state.pointLight.quadratic);

		shader->setVec3("pointLights[0].colors.ambient", state.pointLight.color.ambient);
		shader->setVec3("pointLights[0].colors.diffuse", state.pointLight.color.diffuse);
		shader->setVec3("pointLights[0].colors.specular", state.pointLight.color.specular);

		//SPOT LIGHT 
		shader->setBool("spotLights[0].isActive", state.spotLights[0].active);

		shader->setVec3("spotLights[0].position", state.spotLights[0].pos);
		shader->setVec3("spotLights[0].direction", state.spotLights[0].dir);

		shader->setFloat("spotLights[0].att.constant", state.spotLights[0].constant);
		shader->setFloat("spotLights[0].att.linear", state.spotLights[0].linear);
		shader->setFloat("spotLights[0].att.quadratic", state.spotLights[0].quadratic);

		shader->setVec3("spotLights[0].colors.ambient", state.spotLights[0].color.ambient);
		shader->setVec3("spotLights[0].colors.diffuse", state.spotLights[0].color.diffuse);
		shader->setVec3("spotLights[0].colors.specular", state.spotLights[0].color.specular);

		shader->setFloat("spotLights[0].cutOff", spotLightCutOff[0]);
		shader->setFloat("spotLights[0].outerCutOff", spotLightOuterCutOff[0]);

		//SPOT LIGHT 
		shader->setBool("spotLights[1].isActive", state.spotLights[1].active);

		shader->setVec3("spotLights[1].position", state.spotLights[1].pos);
		shader->setVec3("spotLights[1].direction", state.spotLights[1].dir);
		shader->setFloat("spotLights[1].att.constant", state.spotLights[1].constant);
		shader->setFloat("spotLights[1].att.linear", state.spotLights[1].linear);
		shader->setFloat("spotLights[1].att.quadratic", state.spotLights[1].quadratic);

		shader->setVec3("spotLights[1].colors.ambient", state.spotLights[1].color.ambient);
		shader->setVec3("spotLights[1].colors.diffuse", state.spotLights[1].color.diffuse);
		shader->setVec3("spotLights[1].colors.specular", state.spotLights[1].color.specular);

		shader->setFloat("spotLights[1].cutOff", spotLightCutOff[1]);
		shader->setFloat("spotLights[1].outerCutOff", spotLightOuterCutOff[1]);

	}
}