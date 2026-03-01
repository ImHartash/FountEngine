#include "CInputSystem.hpp"

CInputSystem& CInputSystem::GetInstance() {
	static CInputSystem Instance;
	return Instance;
}

void CInputSystem::ProcessKeyMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// TODO: Realisze Input System
}
