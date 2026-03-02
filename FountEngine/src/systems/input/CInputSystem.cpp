#include "CInputSystem.hpp"

CInputSystem& CInputSystem::GetInstance() {
	static CInputSystem Instance;
	return Instance;
}

void CInputSystem::OnKeyDown(UINT uKey) {

}

void CInputSystem::OnKeyUp(UINT uKey) {

}

void CInputSystem::OnMouseMove(LPARAM lParam) {
	
}

void CInputSystem::OnMouseButtonDown(UINT uButton) {

}

void CInputSystem::OnMouseButtonUp(UINT uButton) {

}

bool CInputSystem::IsKeyDown(UINT uKey) const {
	return false;
}

bool CInputSystem::IsKeyPressed(UINT uKey) const {
	return false;
}

bool CInputSystem::IsKeyReleased(UINT uKey) const {
	return false;
}

int CInputSystem::GetMouseX() const {
	return 0;
}

int CInputSystem::GetMouseY() const {
	return 0;
}

bool CInputSystem::IsMouseButtonDown(UINT button) const {
	return false;
}

void CInputSystem::Update() {

}

CInputSystem::CInputSystem() : m_nMouseX(0), m_nMouseY(0) {
	m_vecKeys.fill(false);
	m_vecPrevKeys.fill(false);

	m_vecMouseButtons.fill(false);
	m_vecPrevMouseButtons.fill(false);
}
