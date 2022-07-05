#pragma once
#include <stdexcept>
#include "Windows.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "json.hpp"
#include "Texture2D.h"

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

#pragma pack(push, 4)
template<class Type = DWORD>
class CObfuscation
{
public:
	unsigned char	m_bIsFilled;
	unsigned char	m_bLengthXor32;
	unsigned char	m_bLengthXor8;
	Type			m_tKey;
	unsigned char	m_bIndex;
	Type			m_atValues[sizeof(Type) == 1 ? 3 : 4];
public:
	Type get()
	{
		__try
		{
			if (m_bIsFilled != 1)
				return Type(0);

			Type tResult = sizeof(Type) == 1 ? this->m_atValues[(this->m_bIndex + 1) & 3] : this->m_atValues[this->m_bIndex];
			if (this->m_bLengthXor32)
			{
				for (unsigned char i = 0; i < this->m_bLengthXor32; i++)
				{
					reinterpret_cast<PDWORD>(&tResult)[i] ^= ~(reinterpret_cast<PDWORD>(&this->m_tKey)[i]);
				}
			}
			if (this->m_bLengthXor8 && sizeof(Type) == 1)
			{
				for (unsigned char i = sizeof(Type) - this->m_bLengthXor8; i < sizeof(Type); i++)
				{
					reinterpret_cast<PBYTE>(&tResult)[i] ^= ~(reinterpret_cast<PBYTE>(&this->m_tKey)[i]);
				}
			}
			return tResult;
		}
		__except (true)
		{
			return Type(0);
		}
	}
};
#pragma pack(pop)

namespace Json {
	std::map<std::string, float> GetChampionData();
	std::map<std::string, ID3D11ShaderResourceView*> GetSpellData(ID3D11Device *device);
};
