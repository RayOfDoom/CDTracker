#pragma once
#include "Windows.h"
#include <string>
#include "GameRenderer.h"
#include "Utils.h"
#include "Vector.h"

class Spell {
public:
	char name[50];
	float readyAt;
	ID3D11ShaderResourceView* icon;

	float getCoolDown(float gameTime);
};

class Champion {
public:
	DWORD address = 0;
	char name[50];
	float health;
	float healthBarHeight = 0.f;
	Vector3 pos;
	bool isVisible;
	float atkRange = 0.f;

	Spell spells[6];
};

class Game{
public:
	float gameTime = 0.f;
	Champion champs[10];

	std::map<std::string, ID3D11ShaderResourceView*> SpellData;
	std::map<std::string, float> UnitData;

	std::unique_ptr<GameRenderer> renderer = std::unique_ptr<GameRenderer>(new GameRenderer());
	ImDrawList* overlay;

	Vector2 GetHpBarPos(Champion& champ);

	void DrawCircleFilled(const Vector2& center, float radius, int numPoints, const ImVec4& color) {
		overlay->AddCircleFilled(ImVec2(center.x, center.y), radius, ImColor(color), numPoints);
	}

	void DrawRect(const Vector4& box, const ImVec4& color, float rounding = 0, float thickness = 1.0) {
		overlay->AddRect(ImVec2(box.x, box.y), ImVec2(box.z, box.w), ImColor(color), rounding, 15, thickness);
	}

	void DrawRectFilled(const Vector4& box, const ImVec4& color, float rounding = 0) {
		overlay->AddRectFilled(ImVec2(box.x, box.y), ImVec2(box.z, box.w), ImColor(color), rounding);
	}

	void DrawTxt(const Vector2& pos, const char* text, const ImVec4& color) {
		overlay->AddText(ImVec2(pos.x, pos.y), ImColor(color), text);
	}

	void DrawLine(const Vector2& start, const Vector2& end, float thickness, const ImVec4& color) {
		overlay->AddLine((const ImVec2&)start, (const ImVec2&)end, ImColor(color), thickness);
	}
};