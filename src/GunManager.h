#pragma once

#ifndef GUN_MANAGER_H
#define GUN_MANAGER_H

#include <vector>

class Object;
class Shader;

class GunManager
{
	std::vector<Object*> guns;

	std::vector<int> gunAmmo;

	int chosenGun = 0;

	bool inAnim = false;

	bool isReturning = false; 

	std::vector<float> finalRot;


public:

	Shader* gunShader = nullptr;

	GunManager() = default;

	void AddGun(Object* newGun);

	void Update();

	void DrawGun();

	const int GetAmmo() const;

	void Reload();

	void SwitchGun(int gunNo);

	void Shoot();

	void AnimateGun();

	void InspectGun();
};

#endif