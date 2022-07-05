#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include "Overlay.h"
#include <filesystem>

DWORD Mem::ReadDWORD(HANDLE hProcess, DWORD addr) {
	DWORD_PTR ptr = NULL;
	SIZE_T bytesRead = 0;
	ReadProcessMemory(hProcess, (DWORD*)addr, &ptr, 4, &bytesRead);
	return ptr;
}

void Mem::Read(HANDLE hProcess, DWORD addr, void* structure, int size) {
	SIZE_T bytesRead = 0;
	ReadProcessMemory(hProcess, (DWORD*)addr, structure, size, &bytesRead);
}

DWORD Mem::ReadDWORDFromBuffer(void* buff, int position) {
	DWORD result;
	memcpy(&result, (char*)buff + position, 4);
	return result;
}

std::string Character::ToLower(std::string str)
{
	std::string strLower;
	strLower.resize(str.size());

	std::transform(str.begin(),
		str.end(),
		strLower.begin(),
		::tolower);

	return strLower;
}

std::string Character::Format(const char* c, const char* args...) {
	char buff[200];
	sprintf_s(buff, c, args);

	return std::string(buff);
}

std::string Character::RandomString(const int len) {

	std::string tmp_s;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	srand((unsigned int)time(0));
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i)
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


	return tmp_s;
}

std::string Character::TwoCharCD(float cd) {
	if (cd < 1) {
		int dec = (int)(cd * 10) % 10;
		return "." + std::to_string(dec);
	}
	else if (cd < 10) {
		return "0" + std::to_string((int)cd);
	}
	else if (cd < 100) {
		return std::to_string((int)cd);
	}
	else {
		return std::to_string((int)std::round(cd / 60)) + "m";
	}
}



std::map<std::string, float> Json::GetChampionData() {
	std::ifstream fJson("UnitData.json");
	std::stringstream buffer;
	buffer << fJson.rdbuf();
	auto json = nlohmann::json::parse(buffer.str());

	std::map<std::string, float> out;

	for (auto unit : json) {
		bool isChamp = false;
		for (auto tag : unit["tags"])
			if (std::string(tag) == "Unit_Champion")
				isChamp = true;
		if (isChamp)
			out.insert({ std::string(unit["name"]), (float)unit["healthBarHeight"] });
	}

	return out;
}

std::map<std::string, ID3D11ShaderResourceView*> Json::GetSpellData(ID3D11Device *device) {
	std::ifstream fJson("SpellData.json");
	std::stringstream buffer;
	buffer << fJson.rdbuf();
	auto json = nlohmann::json::parse(buffer.str());

	std::map<std::string, ID3D11ShaderResourceView*> out;

	for (auto spell : json) {
		if (std::string(spell["icon"]) != "") {
			if (std::filesystem::exists((std::string("C:\\Users\\rayb2\\OneDrive\\3. Other\\CDTracker\\CDTracker\\icons_spells\\") + std::string(Character::ToLower(spell["icon"])) + std::string(".png")).c_str())) {
				out.insert({ Character::ToLower(std::string(spell["name"])), Texture2D::LoadFromFile(device, (std::string("C:\\Users\\rayb2\\OneDrive\\3. Other\\CDTracker\\CDTracker\\icons_spells\\") + std::string(Character::ToLower(spell["icon"])) + std::string(".png")).c_str())->resourceView });			}
			else {
				out.insert({ Character::ToLower(std::string(spell["name"])), Texture2D::LoadFromFile(device, (std::string("C:\\Users\\rayb2\\OneDrive\\3. Other\\CDTracker\\CDTracker\\icons_spells\\summoner_empty.png").c_str()))->resourceView });
			}
		}
	}
	return out;
}