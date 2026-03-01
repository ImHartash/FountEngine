#include "CGraphicsContext.hpp"
#include "utils/defines.hpp"

CGraphicsContext::~CGraphicsContext() {
	RELEASE_COM(m_pDeviceContext);
	RELEASE_COM(m_pDevice);
}

bool CGraphicsContext::Initialize(HWND hWnd, int nWidth, int nHeight) {
	return true;
}

void CGraphicsContext::UpdateScene(float flDeltaTime) {

}

void CGraphicsContext::RenderScene() {

}

void CGraphicsContext::ResizeBuffers(int nWidth, int nHeight) {

}
