#include "CApplication.hpp"

static CApplication* g_pApplication;

LRESULT CALLBACK OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return g_pApplication->HandleMessage(hWnd, uMsg, wParam, lParam);
}

CApplication::CApplication() : m_hInstance(nullptr), m_hMainWindow(nullptr), m_nClientWidth(0), m_nClientHeight(0), m_bPaused(false) {
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
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
