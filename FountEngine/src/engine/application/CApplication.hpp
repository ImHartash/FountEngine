#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include "game/timer/CGameTimer.hpp"
#include "engine/graphicscontext/CGraphicsContext.hpp"

class CApplication {
public:
	CApplication();
	~CApplication();

	CApplication(const CApplication&) = delete;
	CApplication& operator=(const CApplication&) = delete;

	bool Initialize(HINSTANCE hInstance, const std::wstring& wstrWindowCaption, int nWidth = 1280, int nHeight = 720);
	int Run();
	
	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	std::unique_ptr<CGraphicsContext> m_pGraphicsContext;

	HINSTANCE m_hInstance;
	HWND m_hMainWindow;

	int m_nClientWidth;
	int m_nClientHeight;

	CGameTimer m_AppTimer;
	
	bool m_bPaused;
	bool m_bMinimized;
	bool m_bMaximized;
	bool m_bResizing;

	std::wstring m_wstrWindowCaption;
};