#pragma once
#include <Windows.h>
#include <d3d11.h>

class CGraphicsContext {
public:
	CGraphicsContext() = default;
	~CGraphicsContext();

	CGraphicsContext(const CGraphicsContext&) = delete;
	CGraphicsContext operator=(const CGraphicsContext&) = delete;

	bool Initialize(HWND hWnd, int nWidth, int nHeight);

	void UpdateScene(float flDeltaTime);
	void RenderScene();

	void ResizeBuffers(int nWidth, int nHeight);

private:
	// DX11
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;

	// Buffers
	ID3D11Buffer* m_pDepthStencilBuffer;
};