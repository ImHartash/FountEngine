#pragma once
#include <Windows.h>

class CInputSystem {
public:
	static CInputSystem& GetInstance();

	void ProcessKeyMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool IsKeyDown(int key) const;

private:
	CInputSystem();
	~CInputSystem();
};