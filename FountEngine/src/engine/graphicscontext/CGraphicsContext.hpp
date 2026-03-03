#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "game/camera/CCameraObject.hpp"

class CGraphicsContext {
public:
	CGraphicsContext() = default;
	~CGraphicsContext();

	CGraphicsContext(const CGraphicsContext&) = delete;
	CGraphicsContext operator=(const CGraphicsContext&) = delete;

	bool Initialize(HWND hWnd, int nWidth, int nHeight);
	bool IsValidDevice() const { return m_pDevice != nullptr; }

	void UpdateScene(float flDeltaTime);
	void RenderScene();

	void OnResize(int nWidth, int nHeight);

private:
	bool LoadShaders();
	void BuildVertexLayout(ID3DBlob* pVSBlob);

	CCameraObject m_PlayerCamera;
	DirectX::XMFLOAT4X4 m_mtProjection;

	// DX11
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;

	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepthStencilBuffer;

	D3D11_VIEWPORT m_dxViewport;
	ID3D11InputLayout* m_pInputLayout;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Buffer* m_pWorldViewProjectionBuffer;

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
};