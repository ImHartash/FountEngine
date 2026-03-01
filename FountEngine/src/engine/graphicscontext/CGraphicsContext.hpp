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
	bool IsValidDevice() const { return m_pDevice != nullptr; };

	void UpdateScene(float flDeltaTime);
	void RenderScene();

	void OnResize(int nWidth, int nHeight);

private:
	// DX11
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;

	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepthStencilBuffer;

	D3D11_VIEWPORT m_dxViewport;
};