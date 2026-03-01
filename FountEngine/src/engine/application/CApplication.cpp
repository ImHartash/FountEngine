#include "CApplication.hpp"

#define MIN_CLIENT_WINDOW_SIZE_X 500
#define MIN_CLIENT_WINDOW_SIZE_Y 600

static CApplication* g_pApplication;

LRESULT CALLBACK OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return g_pApplication->HandleMessage(hWnd, uMsg, wParam, lParam);
}

CApplication::CApplication() 
	: m_hInstance(nullptr), m_hMainWindow(nullptr), m_nClientWidth(0), m_nClientHeight(0), 
	m_bPaused(false), m_bMinimized(false), m_bMaximized(false)
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

	m_pGraphicsContext = std::make_unique<CGraphicsContext>();
	if (!m_pGraphicsContext->Initialize(m_hMainWindow, m_nClientWidth, m_nClientHeight)) {
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
				m_pGraphicsContext->UpdateScene(m_AppTimer.GetDeltaTime());
				m_pGraphicsContext->RenderScene();
			}
			else {
				Sleep(100);
			}
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
		}
		else {
			m_bPaused = false;
			m_AppTimer.Start();
		}
		return 0;
	case WM_SIZE:
		m_nClientWidth = LOWORD(lParam);
		m_nClientHeight = HIWORD(lParam);
		if (m_pGraphicsContext->IsValidDevice()) {
			if (wParam == SIZE_MINIMIZED) {
				m_bPaused = true;
				m_bMaximized = false;
				m_bMinimized = true;
			}
			else if (wParam == SIZE_MAXIMIZED) {
				m_bPaused = false;
				m_bMinimized = false;
				m_bMaximized = true;
				m_pGraphicsContext->OnResize(m_nClientWidth, m_nClientHeight);
			}
			else if (wParam == SIZE_RESTORED) {
				if (m_bMinimized) {
					m_bPaused = false;
					m_bMinimized = false;
					m_pGraphicsContext->OnResize(m_nClientWidth, m_nClientHeight);
				}
				else if (m_bMaximized) {
					m_bPaused = false;
					m_bMaximized = false;
					m_pGraphicsContext->OnResize(m_nClientWidth, m_nClientHeight);
				}
				else if (m_bResizing) {
					// ...
				}
				else {
					m_pGraphicsContext->OnResize(m_nClientWidth, m_nClientHeight);
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
		m_pGraphicsContext->OnResize(m_nClientWidth, m_nClientHeight);
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
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		// INPUT
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		// INPUT
		return 0;
	case WM_MOUSEMOVE:
		// INPUT
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
