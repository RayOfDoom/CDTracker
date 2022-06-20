#pragma once
#include "Windows.h"
#include <string>

class Spell {
public:
	Spell();

	std::string name;
	float readyAt;

	float getCoolDown(float gameTime);
};

class Champion {
public:
	Champion();

	DWORD address = 0;
	std::string name;
	float healthBarHeight = 0.f;
	float xPos = 0.f;
	float yPos = 0.f;
	float zPos = 0.f;

	Spell spells[6];
};

class Game{
public:
	Game();

	float gameTime = 0.f;

	Champion champs[10];
};