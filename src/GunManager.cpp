#include "GunManager.h"
#include "Object.h"


void GunManager::AddGun(Object* newGun)
{
	guns.emplace_back(newGun);
	gunAmmo.emplace_back(10);
	chosenGun = guns.size() - 1;
}

void GunManager::DrawGun()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	guns.at(chosenGun)->Draw();
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

	if (inAnim)
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
	float rot = 0;

	if (isReturning)
		rot = currentRotZ - 1.0f;
	else
		rot = currentRotZ + 1.0f;

	if (rot >= FINAL_ROT)
	{
		rot = FINAL_ROT;
		isReturning = true;
	}

	if (rot <= -10)
	{
		rot =-10;
		isReturning = false;
		inAnim = false;
	}

	guns.at(chosenGun)->transform.SetLocalRotationZ(rot);

}

void GunManager::Update()
{
	if (inAnim)
	{
		AnimateGun();
	}

	guns.at(chosenGun)->transform.Update();
}


