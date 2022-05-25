#include "GunManager.h"
#include "Object.h"

void GunManager::AddGun(Object* newGun)
{
	guns.emplace_back(newGun);
	gunAmmo.emplace_back(10);
}

void GunManager::DrawGun()
{
	guns.at(chosenGun)->Draw();
}

const int GunManager::GetAmmo(int gunNo) const
{
	return gunAmmo[gunNo];
}

void GunManager::Reload()
{
	gunAmmo.at(chosenGun) = 10;
}

void GunManager::SwitchGun()
{
	(chosenGun > (guns.size() - 1)) ? chosenGun-- : chosenGun++;
}

void GunManager::Shoot()
{
	gunAmmo.at(chosenGun)--;

	//animate gun
	

}


