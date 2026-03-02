#pragma once
#include <Windows.h>
#include <array>
#define KEY_COUNT 256
#define MOUSE_BUTTON_COUNT 5

class CInputSystem {
public:
	static CInputSystem& GetInstance();

	void OnKeyDown(UINT uKey);
	void OnKeyUp(UINT uKey);

	void OnMouseMove(LPARAM lParam);
	void OnMouseButtonDown(UINT uButton);
	void OnMouseButtonUp(UINT uButton);

	bool IsKeyDown(UINT uKey) const;
	bool IsKeyPressed(UINT uKey) const;
	bool IsKeyReleased(UINT uKey) const;

	int GetMouseX() const;
	int GetMouseY() const;
	bool IsMouseButtonDown(UINT button) const;

	void Update();

private:
	std::array<bool, KEY_COUNT> m_vecKeys;
	std::array<bool, KEY_COUNT> m_vecPrevKeys;

	std::array<bool, MOUSE_BUTTON_COUNT> m_vecMouseButtons;
	std::array<bool, MOUSE_BUTTON_COUNT> m_vecPrevMouseButtons;

	int m_nMouseX;
	int m_nMouseY;

	CInputSystem();
	~CInputSystem() = default;
};