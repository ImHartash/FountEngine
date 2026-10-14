#pragma once
#include <Windows.h>
#include <array>

#define KEY_COUNT 256
#define MOUSE_BUTTON_COUNT 6

class CInputSystem {
public:
	CInputSystem();
	~CInputSystem() = default;

	void Initialize(HWND hWnd);

	bool IsKeyDown(UINT uKey) const;
	bool IsKeyPressed(UINT uKey) const;
	bool IsKeyReleased(UINT uKey) const;

	bool IsMouseButtonDown(UINT nKey) const;
	bool IsMouseButtonPressed(UINT nKey) const;
	bool IsMouseButtonReleased(UINT nKey) const;

	void ProcessRawInput(WPARAM wParam, LPARAM lParam);

	float GetMouseDeltaX() const;
	float GetMouseDeltaY() const;
	float GetMouseWheelDelta() const;

	void LockCursor();
	void UnlockCursor();

	bool IsCursorLocked() const;

	void Update();

private:
	void OnKeyDown(UINT uKey);
	void OnKeyUp(UINT uKey);

	void OnMouseDown(UINT nMouseButton);
	void OnMouseUp(UINT nMouseButton);

	RAWINPUT m_RawInputBuffer{};

	std::array<bool, KEY_COUNT> m_vecKeys;
	std::array<bool, KEY_COUNT> m_vecPrevKeys;

	std::array<bool, MOUSE_BUTTON_COUNT> m_vecMouseButtons;
	std::array<bool, MOUSE_BUTTON_COUNT> m_vecPrevMouseButtons;

	float m_flMouseDeltaX;
	float m_flMouseDeltaY;
	float m_flMouseWheelDelta;

	bool m_bCursorLocked;

	HWND m_hWnd;
};