#include "MemoryReader.h"
#include "Offsets.h"
#include <map>

MemoryReader::MemoryReader() {};

bool MemoryReader::IsActive() {
	HWND handle = GetForegroundWindow();
	DWORD h;
	GetWindowThreadProcessId(handle, &h);
	return pid == h;
}

bool MemoryReader::IsHooked() {
	HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
	DWORD ret = WaitForSingleObject(process, 0);
	CloseHandle(process);
	return ret == WAIT_TIMEOUT;
}

void MemoryReader::Hook() {
	hWindow = FindWindowA(NULL, "League of Legends (TM) Client");
	if (hWindow == NULL) {
		throw WinApiException("Couldn't get league window");
	}

	GetWindowThreadProcessId(hWindow, &pid);
	if (pid == NULL) {
		throw WinApiException("Couldn't get league process");
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL) {
		throw WinApiException("Couldn't open league process");
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
		moduleBaseAddr = (DWORD_PTR)hMods[0];
	}
	else {
		throw WinApiException("Couldn't get base address");
	}
}

void MemoryReader::UpdateGameTime(Game& game) {
	Mem::Read(hProcess, moduleBaseAddr + Offsets::GameTime, &game.gameTime, sizeof(game.gameTime));
}

void MemoryReader::ReadRenderer(Game& game) {

	DWORD rendererAddr = Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::Renderer);
	game.renderer->LoadFromMem(rendererAddr, moduleBaseAddr, hProcess);
}

void MemoryReader::ReadSpells(Game& game, Champion& champ) {
	for (int i = 0; i < 6; i++) {
		DWORD spellBookSpellSlot = Mem::ReadDWORD(hProcess, champ.address + Offsets::SpellBookSpellSlot + (0x4 * i));
		DWORD spellSlotSpellInfo = Mem::ReadDWORD(hProcess, spellBookSpellSlot + Offsets::SpellSlotSpellInfo);
		DWORD spellInfoSpellData = Mem::ReadDWORD(hProcess, spellSlotSpellInfo + Offsets::SpellInfoSpellData);
		DWORD spellDataSpellName = Mem::ReadDWORD(hProcess, spellInfoSpellData + Offsets::SpellDataSpellName);

		Mem::Read(hProcess, spellBookSpellSlot + Offsets::SpellSlotTime, &champ.spells[i].readyAt, sizeof(float));
		Mem::Read(hProcess, spellDataSpellName, &champ.spells[i].name, 50);
		champ.spells[i].icon = game.SpellData[champ.spells[i].name];
	}
}

void MemoryReader::ReadChamps(Game& game) {
	DWORD list = Mem::ReadDWORD(hProcess, Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::HeroList)  + 0x4);
	if (list != 0) {
		for (int i = 0; i < 10; i++) {
			game.champs[i].address = Mem::ReadDWORD(hProcess, list + (0x4 * i));
			if (game.champs[i].address != 0) {
				Mem::Read(hProcess, game.champs[i].address + Offsets::ObjName, &game.champs[i].name, 50);
				Mem::Read(hProcess, game.champs[i].address + Offsets::ObjPos, &game.champs[i].pos, sizeof(float) * 3);
				Mem::Read(hProcess, game.champs[i].address + Offsets::ObjVisibility, &game.champs[i].isVisible, sizeof(bool));
				Mem::Read(hProcess, game.champs[i].address + Offsets::ObjHealth, &game.champs[i].health, sizeof(float));
				Mem::Read(hProcess, game.champs[i].address + Offsets::ObjAtkRange, &game.champs[i].atkRange, sizeof(float));
				game.champs[i].healthBarHeight = game.UnitData[Character::ToLower(game.champs[i].name)];

				ReadSpells(game, game.champs[i]);
			}
			else break;
		}
	}
}
