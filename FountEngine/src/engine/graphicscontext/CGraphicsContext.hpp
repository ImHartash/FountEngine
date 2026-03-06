#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

class CGraphicsContext {
public:
	static CGraphicsContext& GetInstance();

	bool Initialize(HWND hWnd, int nWidth, int nHeight);

	void Update(float flDeltaTime);
	void Render();
	void OnResize(int nWidth, int nHeight);

	ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
	ID3D11Device* GetDevice() { return m_pDevice; }
	bool IsValid() const { return m_pDevice != nullptr; }

	DirectX::XMFLOAT4X4& GetProjectionMatrix();

private:
	CGraphicsContext() = default;
	~CGraphicsContext();

	CGraphicsContext(const CGraphicsContext&) = delete;
	CGraphicsContext operator=(const CGraphicsContext&) = delete;

	// Matrices
	DirectX::XMFLOAT4X4 m_mtProjection;

	// DX11
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;

	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepthStencilBuffer;

	D3D11_VIEWPORT m_dxViewport;
};