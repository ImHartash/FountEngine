#include "CInputSystem.hpp"
#include <windowsx.h>

#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"

// UNDER THE DEVELOPING
// I'LL DO THIS LATER/
// - ImHartash :D

void CInputSystem::Initialize(HWND hWnd) {
	m_hWnd = hWnd;

	RAWINPUTDEVICE RID[2];
	RID[0].usUsagePage = 0x01; // Mouse
	RID[0].usUsage = 0x02;
	RID[0].dwFlags = 0;
	RID[0].hwndTarget = hWnd;

	RID[1].usUsagePage = 0x01; // Keyboard
	RID[1].usUsage = 0x06;
	RID[1].dwFlags = 0;
	RID[1].hwndTarget = hWnd;

	RegisterRawInputDevices(RID, 2, sizeof(RAWINPUTDEVICE));
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

bool CInputSystem::IsMouseButtonDown(UINT nKey) const {
	return nKey < MOUSE_BUTTON_COUNT && m_vecMouseButtons[nKey];
}

bool CInputSystem::IsMouseButtonPressed(UINT nKey) const {
	return nKey < MOUSE_BUTTON_COUNT && m_vecMouseButtons[nKey] && !m_vecPrevMouseButtons[nKey];
}

bool CInputSystem::IsMouseButtonReleased(UINT nKey) const {
	return nKey < MOUSE_BUTTON_COUNT && !m_vecMouseButtons[nKey] && m_vecPrevMouseButtons[nKey];
}

void CInputSystem::ProcessRawInput(WPARAM wParam, LPARAM lParam) {	
	UINT nSize = sizeof(RAWINPUT);
	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &m_RawInputBuffer, &nSize, sizeof(RAWINPUTHEADER)) > nSize) {
		return;
	}

	if (m_RawInputBuffer.header.dwType == RIM_TYPEMOUSE) {
		const RAWMOUSE* pMouse = &m_RawInputBuffer.data.mouse;
		if (!(pMouse->usFlags & MOUSE_MOVE_ABSOLUTE)) {
			m_flMouseDeltaX += pMouse->lLastX;
			m_flMouseDeltaY += pMouse->lLastY;
		}

		if (pMouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) OnMouseDown(VK_LBUTTON);
		if (pMouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) OnMouseUp(VK_LBUTTON);

		if (pMouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) OnMouseDown(VK_RBUTTON);
		if (pMouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) OnMouseUp(VK_RBUTTON);

		if (pMouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) OnMouseDown(VK_MBUTTON);
		if (pMouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) OnMouseUp(VK_MBUTTON);

		if (pMouse->usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) OnMouseDown(VK_XBUTTON1);
		if (pMouse->usButtonFlags & RI_MOUSE_BUTTON_4_UP) OnMouseUp(VK_XBUTTON1);

		if (pMouse->usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) OnMouseDown(VK_XBUTTON2);
		if (pMouse->usButtonFlags & RI_MOUSE_BUTTON_5_UP) OnMouseUp(VK_XBUTTON2);
	}
	else if (m_RawInputBuffer.header.dwType == RIM_TYPEKEYBOARD) {
		const RAWKEYBOARD* pKeyboard = &m_RawInputBuffer.data.keyboard;
		if (pKeyboard->Flags & RI_KEY_BREAK) OnKeyUp(pKeyboard->VKey);
		else OnKeyDown(pKeyboard->VKey);
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

bool CInputSystem::IsCursorLocked() const {
	return m_bCursorLocked;
}

void CInputSystem::Update() {
	m_vecPrevKeys = m_vecKeys;
	m_vecPrevMouseButtons = m_vecMouseButtons;

	m_flMouseDeltaX = 0.0f;
	m_flMouseDeltaY = 0.0f;
}

void CInputSystem::OnKeyDown(UINT uKey) {
	if (uKey < KEY_COUNT)
		m_vecKeys[uKey] = true;
}

void CInputSystem::OnKeyUp(UINT uKey) {
	if (uKey < KEY_COUNT)
		m_vecKeys[uKey] = false;
}

void CInputSystem::OnMouseDown(UINT nMouseButton) {
	if (nMouseButton < MOUSE_BUTTON_COUNT)
		m_vecMouseButtons[nMouseButton] = true;
}

void CInputSystem::OnMouseUp(UINT nMouseButton) {
	if (nMouseButton < MOUSE_BUTTON_COUNT)
		m_vecMouseButtons[nMouseButton] = false;
}

CInputSystem::CInputSystem() 
	: m_flMouseDeltaX(0.0f), m_flMouseDeltaY(0.0f),
	m_bCursorLocked(false), m_hWnd(nullptr) {
	m_vecKeys.fill(false);
	m_vecPrevKeys.fill(false);

	m_vecMouseButtons.fill(false);
	m_vecPrevMouseButtons.fill(false);
}
