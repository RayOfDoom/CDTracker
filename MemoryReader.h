#pragma once
#include "Windows.h"
#include "psapi.h"
#include "Utils.h"
#include "Objects.h"

class MemoryReader {
public:
	MemoryReader();

	bool IsHooked();
	bool IsActive();
	void Hook();
	void UpdateGameTime(Game& game);
	void ReadRenderer(Game& game);
	void ReadSpells(Game& game, Champion& champ);
	void ReadChamps(Game& game);

private:
	HANDLE hProcess = NULL;
	DWORD pid = 0;
	HWND hWindow = NULL;
	DWORD_PTR moduleBaseAddr = 0;
	DWORD moduleSize = 0;
};