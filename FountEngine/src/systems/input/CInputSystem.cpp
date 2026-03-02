#include "CInputSystem.hpp"
#include <windowsx.h>

#include "systems/logging/CLogSystem.hpp"

// UNDER THE DEVELOPING
// I'LL DO THIS LATER/
// - ImHartash :D

CInputSystem& CInputSystem::GetInstance() {
	static CInputSystem Instance;
	return Instance;
}

void CInputSystem::Initialize(HWND hWnd) {
	m_hWnd = hWnd;

	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = hWnd;

	RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

void CInputSystem::OnKeyDown(UINT uKey) {
	if (uKey < KEY_COUNT)
		m_vecKeys[uKey] = true;
}

void CInputSystem::OnKeyUp(UINT uKey) {
	if (uKey < KEY_COUNT)
		m_vecKeys[uKey] = false;
}

bool CInputSystem::IsKeyDown(UINT uKey) const {
	return uKey < KEY_COUNT && m_vecKeys[uKey];
}

bool CInputSystem::IsKeyPressed(UINT uKey) const {
	return uKey < KEY_COUNT && m_vecKeys[uKey] && !m_vecPrevKeys[uKey];
}

bool CInputSystem::IsKeyReleased(UINT uKey) const {
	return uKey < KEY_COUNT && !m_vecKeys[uKey] && m_vecPrevKeys[uKey];
}

void CInputSystem::ProcessRawInput(LPARAM lParam) {
	UINT uSize = 0;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &uSize, sizeof(RAWINPUTHEADER));
	if (uSize == 0)
		return;

	std::vector<BYTE> vecBuffer(uSize);
	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, vecBuffer.data(), &uSize, sizeof(RAWINPUTHEADER)) != uSize)
		return;

	RAWINPUT* pRaw = reinterpret_cast<RAWINPUT*>(vecBuffer.data());
	if (pRaw->header.dwType == RIM_TYPEMOUSE) {
		m_flMouseDeltaX += static_cast<float>(pRaw->data.mouse.lLastX);
		m_flMouseDeltaY += static_cast<float>(pRaw->data.mouse.lLastY);
	}

	if (m_bCursorLocked) {
		RECT rArea;
		GetClientRect(m_hWnd, &rArea);

		POINT ptCenter;
		ptCenter.x = (rArea.right - rArea.left) / 2;
		ptCenter.y = (rArea.bottom - rArea.top) / 2;

		ClientToScreen(m_hWnd, &ptCenter);
		SetCursorPos(ptCenter.x, ptCenter.y);
	}
}

float CInputSystem::GetMouseDeltaX() const {
	return m_flMouseDeltaX;
}

float CInputSystem::GetMouseDeltaY() const {
	return m_flMouseDeltaY;
}

void CInputSystem::LockCursor() {
	if (m_bCursorLocked)
		return;

	ShowCursor(FALSE);

	RECT rArea;
	GetClientRect(m_hWnd, &rArea);

	POINT ul = { rArea.left, rArea.top };
	POINT lr = { rArea.right, rArea.bottom };

	ClientToScreen(m_hWnd, &ul);
	ClientToScreen(m_hWnd, &lr);

	rArea.left = ul.x;
	rArea.top = ul.y;
	rArea.right = lr.x;
	rArea.bottom = lr.y;

	ClipCursor(&rArea);

	m_bCursorLocked = true;
}

void CInputSystem::UnlockCursor() {
	if (!m_bCursorLocked)
		return;

	ShowCursor(TRUE);
	ClipCursor(nullptr);

	m_bCursorLocked = false;
}

void CInputSystem::Update() {
	m_vecPrevKeys = m_vecKeys;

	m_flMouseDeltaX = 0.0f;
	m_flMouseDeltaY = 0.0f;
}

CInputSystem::CInputSystem() : m_flMouseDeltaX(0.0f), m_flMouseDeltaY(0.0f), m_bCursorLocked(false), m_hWnd(nullptr) {
	m_vecKeys.fill(false);
	m_vecPrevKeys.fill(false);

	m_vecMouseButtons.fill(false);
	m_vecPrevMouseButtons.fill(false);
}
