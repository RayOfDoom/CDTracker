#pragma once
#pragma once
#include "windows.h"
#include "ImGui/imgui.h"
#include "Vector.h"

/// Represents the state of the games renderer
class GameRenderer {

public:
	int width, height;

	float viewMatrix[16];
	float projMatrix[16];
	float viewProjMatrix[16];

	void LoadFromMem(DWORD_PTR renderBase, DWORD_PTR moduleBase, HANDLE hProcess);

	Vector2 WorldToScreen(const Vector3& pos) const;

	Vector2 WorldToMinimap(const Vector3& pos, const Vector2& wPos, const Vector2& wSize) const;

	float DistanceToMinimap(float dist, const Vector2& wSize) const;

	void DrawCircleAt(ImDrawList* canvas, const Vector3& worldPos, float radius, bool filled, int numPoints, ImColor color, float thickness = 3.f) const;

	bool IsScreenPointOnScreen(const Vector2& point, float offsetX = 0.f, float offsetY = 0.f) const;

	bool IsWorldPointOnScreen(const Vector3& point, float offsetX = 0.f, float offsetY = 0.f) const;

private:
	void MultiplyMatrices(float* out, float* a, int row1, int col1, float* b, int row2, int col2);
};