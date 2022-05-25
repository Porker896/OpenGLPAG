#pragma once

#ifndef GUN_MANAGER_H
#define GUN_MANAGER_H

#include <vector>

class Object;

class GunManager
{
	std::vector<Object*> guns;

	std::vector<int> gunAmmo;

	int chosenGun = 0;


public:

	GunManager() = default;

	void AddGun(Object* newGun);

	void Update();

	void DrawGun();

	const int GetAmmo(int gunNo) const;

	void Reload();

	void SwitchGun();

	void Shoot();
};

#endif