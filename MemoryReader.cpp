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
	hWindow = FindWindowA("RiotWindowClass", NULL);
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

void MemoryReader::ReadSpells(Game& game, Champion& champ) {
	for (int i = 0; i < 6; i++) {
		DWORD spellBookSpellSlot = Mem::ReadDWORD(hProcess, champ.address + Offsets::ObjSpellBook + Offsets::SpellBookSpellSlot + (0x4 * i));
		DWORD spellSlotSpellInfo = Mem::ReadDWORD(hProcess, spellBookSpellSlot + Offsets::SpellSlotSpellInfo);
		DWORD spellInfoSpellData = Mem::ReadDWORD(hProcess, spellSlotSpellInfo + Offsets::SpellInfoSpellData);
		DWORD spellDataSpellName = Mem::ReadDWORD(hProcess, spellInfoSpellData + Offsets::SpellDataSpellName);

		float readyAt = 0.f;
		Mem::Read(hProcess, spellBookSpellSlot + Offsets::SpellSlotTime, &readyAt, sizeof(readyAt));

		char name[50];
		Mem::Read(hProcess, spellDataSpellName, &name, 50);

		champ.spells[i].name = name;
		champ.spells[i].readyAt = readyAt;
	}
}

void MemoryReader::ReadChamps(Game& game) {
	DWORD list = Mem::ReadDWORD(hProcess, Mem::ReadDWORD(hProcess, moduleBaseAddr + Offsets::HeroList)  + 0x4);
	if (list != 0) {
		for (int i = 0; i < 10; i++) {
			DWORD champAddress = Mem::ReadDWORD(hProcess, list + (0x4 * i));
			if (champAddress != 0) {

				char name[50];
				Mem::Read(hProcess, Mem::ReadDWORD(hProcess, champAddress + Offsets::ObjName), name, 50);
				//std::cout << std::string(name) << std::endl;

				//TODO:GET POSITION AND HPBARHEIGHT

				game.champs[i].address = champAddress;
				game.champs[i].name = name;

				ReadSpells(game, game.champs[i]);
			}
			else break;
		}
	}
}
