#pragma once
#include <stdexcept>
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "json.hpp"


class WinApiException : public std::runtime_error {

public:
	WinApiException(const char* message)
		:std::runtime_error(message) {

		errorCode = GetLastError();
		this->message = message;
	}

private:
	const char* message = nullptr;
	int errorCode = 0;

};

namespace Mem {
	DWORD ReadDWORD(HANDLE hProcess, DWORD addr);
	void Read(HANDLE hProcess, DWORD addr, void* structure, int size);
	DWORD ReadDWORDFromBuffer(void* buff, int position);
};

namespace Character {
	std::string ToLower(std::string str);
	std::string RandomString(const int len);
	std::string Format(const char* c, const char* args...);
	std::string TwoCharCD(float cd);
};

namespace Json {
	std::map<std::string, float> GetChampionData();
};
