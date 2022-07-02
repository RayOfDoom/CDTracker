#include "Overlay.h"
#include "Utils.h"

#define HCheck(x, m) if(x != S_OK) { throw std::runtime_error(Character::Format("DirectX: Failed at %s. Error code: %d\n", m, MAKE_HRESULT(1, _FACDXGI, x))); } //X?

ID3D11Device* Overlay::dxDevice = NULL;
ID3D11DeviceContext* Overlay::dxDeviceContext = NULL;
IDXGISwapChain1* Overlay::dxSwapChain = NULL;
ID3D11RenderTargetView* Overlay::dxRenderTarget = NULL;

Overlay::Overlay() {};

void Overlay::Init() {
	std::string windowClassName = Character::RandomString(10);
	std::string windowName = Character::RandomString(10);
	SetConsoleTitleA(windowName.c_str());

	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, windowClassName.c_str(), NULL };
	RegisterClassExA(&wc);
	hWindow = CreateWindowExA(
		WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
		windowClassName.c_str(), windowName.c_str(),
		WS_POPUP,
		1, 1, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		nullptr, nullptr, GetModuleHandle(0), nullptr
	);

	if (hWindow == NULL) {
		throw WinApiException("Failed to create overlay");
	}

	ShowWindow(hWindow, SW_SHOW);

	if (!CreateDeviceD3D(hWindow)) {
		CleanupDeviceD3D();
		throw std::runtime_error("Failed to create D3D device");
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("NaturalMonoRegular-9YBeK.ttf", 20);
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWindow);
	ImGui_ImplDX11_Init(dxDevice, dxDeviceContext);

	ImGui::GetStyle().Alpha = 1.f;
}

void Overlay::StartFrame() {
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));
	if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Overlay::Update(Game& game) {
	if (/*GAME IS OVER || */ !isWindowVisible) return;

	if (drawOverlay)
		DrawOverlay(game);
	if (drawUI)
		DrawUI(game);
	//frame time can be calculated here
}

void Overlay::RenderFrame() {
	static ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);

	ImGui::EndFrame();
	ImGui::Render();
	dxDeviceContext->OMSetRenderTargets(1, &dxRenderTarget, NULL);
	dxDeviceContext->ClearRenderTargetView(dxRenderTarget, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	dxSwapChain->Present(0, 0);
	//more benchmark calculations
}

void Overlay::DrawUI(Game& game) {
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 680), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("rayray's cdtracker")) {
		ImGui::End();
		return;
	}
	
	if (ImGui::BeginTabBar("cdtrackerbar", ImGuiTabBarFlags_None)) {
		if (ImGui::BeginTabItem("cdtracker")) {
			ImGui::Text("If your LoL cursor turns into your windows cursor, press F8.\nYou will not be able to click on this panel until you press F8 again.\n\nYou can press F7 to toggle the visibility of this panel.\n\nYou can press F6 to toggle the visibility of the overlay.");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("cdinfo")) {
			DrawCDs(game);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("images")) {
			ImGui::Image(Texture2D::LoadFromFile(dxDevice, std::string("C:\\Users\\rayb2\\OneDrive\\3. Other\\CDTracker\\CDTracker\\icons_spells\\551.png"))->resourceView, ImVec2(100, 100));
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void Overlay::DrawOverlay(Game& game) {
	auto io = ImGui::GetIO();
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("##Overlay", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground
	);

	game.overlay = ImGui::GetWindowDrawList();
	DrawSpells(game);
	ImGui::End();
}

void Overlay::DrawSpells(Game& game) {
	for (int i = 0; i < 10; i++) {
		if (!game.champs[i].isVisible || game.champs[i].health <= 0.5f) continue;
		Vector2 pos = game.GetHpBarPos(game.champs[i]);
		
		for (int j = 0; j < 6; j++) {
			std::string name = Character::ToLower(game.champs[i].spells[j].name);
			float cd = game.champs[i].spells[j].getCoolDown(game.gameTime);
			bool isReady = cd == 0;
			
			float leftX = pos.x - 75 + (24 * j);

			Vector4 box;
			box.x = leftX;
			box.y = pos.y ;
			box.z = leftX + 21;
			box.w = pos.y + 21;

			Vector4 outline;
			outline.x = box.x - 1;
			outline.y = box.y - 1;
			outline.z = box.z + 1;
			outline.w = box.w + 1;

			if (!isReady) {
				game.DrawRectFilled(outline, ImColor(255, 0, 0, 255));
			}
			else {
				game.DrawRectFilled(outline, ImColor(0, 255, 0, 255));
			}

			game.overlay->AddImage((void*)game.SpellData[Character::ToLower(game.champs[i].spells[j].name)], ImVec2(box.x, box.y), ImVec2(box.z, box.w));

			if (!isReady) {
				game.DrawRectFilled(box, ImColor(0, 0, 0, 150));

				Vector2 cdpos;
				cdpos.x = leftX;
				cdpos.y = pos.y + 1;

				game.DrawTxt(cdpos, Character::TwoCharCD(cd).c_str(), ImColor(255, 255, 255, 255));
			}
		}
	}
}

void Overlay::DrawCDs(Game& game) {
	for (Champion champ : game.champs) {
		ImGui::Text("%s: x: %.1f y: %.1f, z: %.1f addr: %x", champ.name, champ.pos.x, champ.pos.y, champ.pos.z, champ.address);
		ImGui::Text("HP bar position on screen: x: %.1f, y: %.1f", game.GetHpBarPos(champ).x, game.GetHpBarPos(champ).y);
		for (Spell spell : champ.spells) {
			float coolDown = spell.getCoolDown(game.gameTime);
			ImGui::Image(game.SpellData[Character::ToLower(spell.name)], ImVec2(30, 30));
			ImGui::Text("%s: %.1f", spell.name, coolDown);
		}
	}	
}

bool Overlay::IsVisible() {
	return isWindowVisible;
}

void Overlay::Hide() {
	ShowWindow(hWindow, SW_HIDE);
	isWindowVisible = false;
}

void Overlay::Show() {
	ShowWindow(hWindow, SW_SHOW);
	isWindowVisible = true;
}

void Overlay::ToggleTransparent() {
	LONG ex_style = GetWindowLong(hWindow, GWL_EXSTYLE);
	ex_style = (ex_style & WS_EX_TRANSPARENT) ? (ex_style & ~WS_EX_TRANSPARENT) : (ex_style | WS_EX_TRANSPARENT);
	SetWindowLong(hWindow, GWL_EXSTYLE, ex_style);
	isTransparent = (ex_style & WS_EX_TRANSPARENT);
}

ID3D11Device* Overlay::GetDxDevice() {
	return dxDevice;
}

bool Overlay::CreateDeviceD3D(HWND hWnd)
{
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HCheck(D3D11CreateDevice(
		nullptr,    // Adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,    // Module
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0, // Highest available feature level
		D3D11_SDK_VERSION,
		&dxDevice,
		nullptr,    // Actual feature level
		&dxDeviceContext), "Creating device");

	IDXGIDevice* dxgiDevice;
	HCheck(dxDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice), "Query DXGI Device");

	IDXGIAdapter* dxgiAdapter = 0;
	HCheck(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter), "Get DXGI Adapter");

	IDXGIFactory2* dxgiFactory = 0;
	HCheck(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory), "Get DXGI Factory");

	// Create swap chain with alpha mode premultiplied
	DXGI_SWAP_CHAIN_DESC1 description = {};
	description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	description.BufferCount = 2;
	description.SampleDesc.Count = 1;
	description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
	description.Scaling = DXGI_SCALING_STRETCH;
	RECT rect = {};
	GetClientRect(hWnd, &rect);
	description.Width = rect.right - rect.left;
	description.Height = rect.bottom - rect.top;

	HCheck(dxgiFactory->CreateSwapChainForComposition(dxgiDevice,
		&description,
		nullptr,
		&dxSwapChain), "Create swap chain");

	IDCompositionDevice* dcompDevice;
	DCompositionCreateDevice(
		dxgiDevice,
		__uuidof(dcompDevice), 
		(void**)&dcompDevice);

	IDCompositionTarget* target;
	dcompDevice->CreateTargetForHwnd(hWnd,
		true,
		&target);

	IDCompositionVisual* visual;
	dcompDevice->CreateVisual(&visual);
	visual->SetContent(dxSwapChain);
	target->SetRoot(visual);
	dcompDevice->Commit();

	CreateRenderTarget();
	return true;
}

void Overlay::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (dxSwapChain) { dxSwapChain->Release(); dxSwapChain = NULL; }
	if (dxDeviceContext) { dxDeviceContext->Release(); dxDeviceContext = NULL; }
	if (dxDevice) { dxDevice->Release(); dxDevice = NULL; }
}

void Overlay::CreateRenderTarget()
{
	ID3D11Resource* pBackBuffer;
	if (S_OK != dxSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))
		throw std::runtime_error("Failed to retrieve DX11 swap chain buffer");
	if (S_OK != dxDevice->CreateRenderTargetView(pBackBuffer, NULL, &dxRenderTarget))
		throw std::runtime_error("Failed to create DX11 render target");
	pBackBuffer->Release();
}

void Overlay::CleanupRenderTarget()
{
	if (dxRenderTarget) { dxRenderTarget->Release(); dxRenderTarget = NULL; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI Overlay::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (dxDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			dxSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}