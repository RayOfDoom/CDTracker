#pragma once
#include "Objects.h"
#include "Windows.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <dinput.h>
#include <dcomp.h>
#include <d3d11_2.h>
#include <dxgi1_3.h>

class Overlay {
public:
	Overlay();

	void Init();
	void StartFrame();
	void Update(Game game);
	void RenderFrame();

	bool IsVisible();
	void Hide();
	void Show();
	void ToggleTransparent();

	static ID3D11Device* GetDxDevice();

private:
	void DrawUI(Game game);
	void DrawOverlay();
	void DrawCDs(Game game);

	static bool    CreateDeviceD3D(HWND hWnd);
	static void    CleanupDeviceD3D();
	static void    CreateRenderTarget();
	static void    CleanupRenderTarget();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	HWND hWindow;
	bool isWindowVisible = true;
	bool drawUI = true;

	static ID3D11Device* dxDevice;
	static ID3D11DeviceContext* dxDeviceContext;
	static IDXGISwapChain1* dxSwapChain;
	static ID3D11RenderTargetView* dxRenderTarget;
};