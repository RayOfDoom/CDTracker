#include "Input.h"
#include "Windows.h"
#include <chrono>

using namespace std;
using namespace chrono;

void Input::PressKey(HKey key) {
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = key;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = 0;
	input.ki.dwFlags = KEYEVENTF_SCANCODE;
	SendInput(1, &input, sizeof(INPUT));

	Sleep(8);
	input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

bool Input::WasKeyPressed(HKey key) {

	static high_resolution_clock::time_point nowTime;
	static high_resolution_clock::time_point lastTimePressed[300] = { high_resolution_clock::now() };
	static bool pressed[300] = { 0 };

	static duration<float, std::milli> timeDiff;

	int virtualKey = MapVirtualKeyA(key, MAPVK_VSC_TO_VK);
	if (virtualKey == 0)
		return false;

	nowTime = high_resolution_clock::now();
	timeDiff = nowTime - lastTimePressed[virtualKey];
	if (pressed[virtualKey]) {

		if (timeDiff.count() > 200)
			pressed[virtualKey] = false;
		return false;
	}

	bool keyDown = GetAsyncKeyState(virtualKey) & 0x8000;
	if (keyDown) {
		lastTimePressed[virtualKey] = high_resolution_clock::now();
		pressed[virtualKey] = true;
		return true;
	}

	return false;
}