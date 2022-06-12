#include "GunManager.h"
#include "Object.h"


void GunManager::AddGun(Object* newGun)
{
	guns.emplace_back(newGun);
	gunAmmo.emplace_back(10);
	chosenGun = static_cast<int>(guns.size()) - 1;
	finalRot.emplace_back(newGun->transform.GetLocalRotation().z + 10);
}

void GunManager::DrawGun()
{
	//glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
	guns.at(chosenGun)->Draw();
	glDepthFunc(GL_LESS);

}

const int GunManager::GetAmmo() const
{
	return gunAmmo[chosenGun];
}

void GunManager::Reload()
{
	gunAmmo.at(chosenGun) = 10;
}

void GunManager::SwitchGun(int gunNo)
{
	chosenGun = gunNo;

	chosenGun = std::clamp(chosenGun, 0, static_cast<int>(guns.size()));

}

void GunManager::Shoot()
{

	if (inAnim || gunAmmo.at(chosenGun) <= 0)
	{
		//reset anim?
		//query anim?
		//ignore for now
		return;
	}


	gunAmmo.at(chosenGun)--;
	inAnim = true;
	isReturning = false;


}

void GunManager::AnimateGun()
{
	const auto currentRotZ = guns.at(chosenGun)->transform.GetLocalRotation().z;
	const auto finalRotZ =  finalRot.at(chosenGun);

	float rot = 0;

	if (isReturning)
		rot = currentRotZ - 1.0f;
	else
		rot = currentRotZ + 1.0f;

	if (rot >= finalRotZ)
	{
		rot = finalRotZ;
		isReturning = true;
	}

	if (rot <= finalRotZ - 10.0f)
	{
		rot = finalRotZ - 10.0f;
		isReturning = false;
		inAnim = false;
	}

	guns.at(chosenGun)->transform.SetLocalRotationZ(rot);

}

void GunManager::InspectGun()
{
	guns.at(chosenGun)->transform.SetLocalRotation(glm::vec3(0.0f));
	guns.at(chosenGun)->transform.SetLocalPosition(glm::vec3(0.0f,0.0f,-5.0f));

}

void GunManager::Update()
{
	if (inAnim)
	{
		AnimateGun();
	}

	guns.at(chosenGun)->transform.Update();
}


