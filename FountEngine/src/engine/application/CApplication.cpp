#include "CApplication.hpp"
#include "engine/renderer/CRenderer.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/entitysystem/CEntitySystem.hpp"
#include "systems/inputsystem/CInputSystem.hpp"
#include "systems/filesystem/CFileSystem.hpp"
#include "systems/postprocesssystem/CPostProcessSystem.hpp"

#define MIN_CLIENT_WINDOW_SIZE_X 500
#define MIN_CLIENT_WINDOW_SIZE_Y 600
#define BOOL_ENABLE_4XMSAA true

static CApplication* g_pApplication;

LRESULT CALLBACK OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return g_pApplication->HandleMessage(hWnd, uMsg, wParam, lParam);
}

CApplication::CApplication() 
	: m_hInstance(nullptr), m_hMainWindow(nullptr), m_nClientWidth(0), m_nClientHeight(0), 
	m_bPaused(false), m_bMinimized(false), m_bMaximized(false), m_bResizing(false), m_bCursorLocked(false)
{
	g_pApplication = this;
}

CApplication::~CApplication() {
	ReleaseCapture();
}

bool CApplication::Initialize(HINSTANCE hInstance, const std::wstring& wstrWindowCaption, int nWidth, int nHeight) {
	m_hInstance = hInstance;
	m_wstrWindowCaption = wstrWindowCaption;
	m_nClientWidth = nWidth;
	m_nClientHeight = nHeight;

	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = OnWndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = m_hInstance;
	WndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(0, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	WndClass.lpszMenuName = 0;
	WndClass.lpszClassName = L"FountEngineWindowClass";

	if (!RegisterClass(&WndClass)) {
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT rArea = { 0, 0, m_nClientWidth, m_nClientHeight };
	AdjustWindowRect(&rArea, WS_OVERLAPPEDWINDOW, false);
	int nWindowWidth = rArea.right - rArea.left;
	int nWindowHeight = rArea.bottom - rArea.top;

	m_hMainWindow = CreateWindow(WndClass.lpszClassName, m_wstrWindowCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, nWindowWidth, nWindowHeight, 0, 0, m_hInstance, 0);
	if (!m_hMainWindow) {
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	// Initializing systems
	g_pFileSystem->Initialize();
	if (!g_pInputSystem->Initialize(m_hMainWindow)) return false;
	g_pEntitySystem->Initialize();

	// Initializing Other
	if (!CGraphicsContext::GetInstance().Initialize(m_hMainWindow, m_nClientWidth, m_nClientHeight, BOOL_ENABLE_4XMSAA)) {
		return false;
	}

	if (!CRenderer::GetInstance().Initialize()) {
		return false;
	}

	ShowWindow(m_hMainWindow, SW_SHOW);
	UpdateWindow(m_hMainWindow);

	return true;
}

int CApplication::Run() {
	MSG uMsg = { 0 };
	m_AppTimer.Reset();

	while (uMsg.message != WM_QUIT) {
		if (PeekMessage(&uMsg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
		else {
			m_AppTimer.Tick();
			if (!m_bPaused) {
				CGraphicsContext::GetInstance().Update(m_AppTimer.GetDeltaTime());
				CGraphicsContext::GetInstance().Render();
			}
			else {
				Sleep(100);
			}

			g_pInputSystem->Update();
		}
	}

	return (int)uMsg.wParam;
}	

LRESULT CApplication::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) {
			m_bPaused = true;
			m_AppTimer.Stop();

			// Unlocking cursor
			m_bCursorLocked = g_pInputSystem->IsCursorLocked();
			if (m_bCursorLocked) g_pInputSystem->UnlockCursor();

			// Clearing key states
			g_pInputSystem->ClearKeyStates();
		}
		else {
			m_bPaused = false;
			m_AppTimer.Start();

			// Locking cursor
			if (m_bCursorLocked) g_pInputSystem->LockCursor();
		}
		return 0;
	case WM_SIZE:
		m_nClientWidth = LOWORD(lParam);
		m_nClientHeight = HIWORD(lParam);
		if (CGraphicsContext::GetInstance().IsValid()) {
			if (wParam == SIZE_MINIMIZED) {
				m_bPaused = true;
				m_bMaximized = false;
				m_bMinimized = true;
			}
			else if (wParam == SIZE_MAXIMIZED) {
				m_bPaused = false;
				m_bMinimized = false;
				m_bMaximized = true;
				CGraphicsContext::GetInstance().OnResize(m_nClientWidth, m_nClientHeight);
				if (g_pInputSystem->IsCursorLocked()) g_pInputSystem->LockCursor();
			}
			else if (wParam == SIZE_RESTORED) {
				if (m_bMinimized) {
					m_bPaused = false;
					m_bMinimized = false;
					CGraphicsContext::GetInstance().OnResize(m_nClientWidth, m_nClientHeight);
					if (g_pInputSystem->IsCursorLocked()) g_pInputSystem->LockCursor();
				}
				else if (m_bMaximized) {
					m_bPaused = false;
					m_bMaximized = false;
					CGraphicsContext::GetInstance().OnResize(m_nClientWidth, m_nClientHeight);
					if (g_pInputSystem->IsCursorLocked()) g_pInputSystem->LockCursor();
				}
				else if (m_bResizing) {
					// ...
				}
				else {
					CGraphicsContext::GetInstance().OnResize(m_nClientWidth, m_nClientHeight);
					if (g_pInputSystem->IsCursorLocked()) g_pInputSystem->LockCursor();
				}
			}
		}
		return 0;
	case WM_ENTERSIZEMOVE:
		m_bPaused = true;
		m_bResizing = true;
		m_AppTimer.Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		m_bPaused = false;
		m_bResizing = false;
		m_AppTimer.Start();
		CGraphicsContext::GetInstance().OnResize(m_nClientWidth, m_nClientHeight);
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = MIN_CLIENT_WINDOW_SIZE_X;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = MIN_CLIENT_WINDOW_SIZE_Y;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_INPUT:
		g_pInputSystem->ProcessRawInput(wParam, lParam);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
