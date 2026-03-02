#pragma once
#include <Windows.h>
#include <array>

#define KEY_COUNT 256
#define MOUSE_BUTTON_COUNT 5

class CInputSystem {
public:
	static CInputSystem& GetInstance();
	void Initialize(HWND hWnd);

	void OnKeyDown(UINT uKey);
	void OnKeyUp(UINT uKey);

	bool IsKeyDown(UINT uKey) const;
	bool IsKeyPressed(UINT uKey) const;
	bool IsKeyReleased(UINT uKey) const;

	void ProcessRawInput(LPARAM lParam);

	float GetMouseDeltaX() const;
	float GetMouseDeltaY() const;

	void LockCursor();
	void UnlockCursor();

	void Update();

private:
	std::array<bool, KEY_COUNT> m_vecKeys;
	std::array<bool, KEY_COUNT> m_vecPrevKeys;

	std::array<bool, MOUSE_BUTTON_COUNT> m_vecMouseButtons;
	std::array<bool, MOUSE_BUTTON_COUNT> m_vecPrevMouseButtons;

	float m_flMouseDeltaX;
	float m_flMouseDeltaY;

	bool m_bCursorLocked;

	HWND m_hWnd;

	CInputSystem();
	~CInputSystem() = default;
};