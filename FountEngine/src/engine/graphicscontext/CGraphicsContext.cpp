#include "CGraphicsContext.hpp"
#include "utils/defines.hpp"
#include "systems/logging/CLogSystem.hpp"
#include "systems/input/CInputSystem.hpp"
#include "math/defines.hpp"

CGraphicsContext::~CGraphicsContext() {
	RELEASE_COM(m_pDeviceContext);
	RELEASE_COM(m_pDevice);
}

bool CGraphicsContext::Initialize(HWND hWnd, int nWidth, int nHeight) {
	UINT uCreateDeviceFlags = 0;
	
#ifdef _DEBUG
	uCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL dxFeatureLevel;
	HRESULT hResult = D3D11CreateDevice(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		uCreateDeviceFlags,
		0, 0,
		D3D11_SDK_VERSION,
		&m_pDevice,
		&dxFeatureLevel,
		&m_pDeviceContext
	);

	if (FAILED(hResult)) {
		LOG_ERROR("Failed to create ID3D11Device.");
		return false;
	}

	if (dxFeatureLevel < D3D_FEATURE_LEVEL_11_0) {
		LOG_ERROR("Direct3D feature level 11 unsupported.");
		return false;
	}
	
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = nWidth;
	sd.BufferDesc.Height = nHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	IDXGIDevice* pDXGIDevice = nullptr;
	HR(m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDXGIDevice)));

	IDXGIAdapter* pDXGIAdapter = nullptr;
	HR(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDXGIAdapter)));

	IDXGIFactory* pDXGIFactory = nullptr;
	HR(pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pDXGIFactory)));

	HR(pDXGIFactory->CreateSwapChain(m_pDevice, &sd, &m_pSwapChain));

	RELEASE_COM(pDXGIDevice);
	RELEASE_COM(pDXGIAdapter);
	RELEASE_COM(pDXGIFactory);

	if (!LoadShaders()) {
		LOG_FATAL("Failed to load shaders...");
		return false;
	}

	OnResize(nWidth, nHeight);

	m_PlayerCamera.SetPosition({ -5.f, 0.f, 0.f });

	return true;
}

void CGraphicsContext::UpdateScene(float flDeltaTime) {
	m_PlayerCamera.Update(flDeltaTime);

	DirectX::XMFLOAT4X4 mtViewMatrix = m_PlayerCamera.GetViewMatrix();
}

void CGraphicsContext::RenderScene() {
	assert(m_pDeviceContext);
	assert(m_pSwapChain);

	const float aClearColor[4] = { 0.2f, 0.4f, 0.7f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, aClearColor);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);



	HR(m_pSwapChain->Present(1, 0));
}

void CGraphicsContext::OnResize(int nWidth, int nHeight) {
	assert(m_pDeviceContext);
	assert(m_pDevice);
	assert(m_pSwapChain);

	RELEASE_COM(m_pRenderTargetView);
	RELEASE_COM(m_pDepthStencilView);
	RELEASE_COM(m_pDepthStencilBuffer);

	HR(m_pSwapChain->ResizeBuffers(1, nWidth, nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* pBackBuffer;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer)));
	HR(m_pDevice->CreateRenderTargetView(pBackBuffer, 0, &m_pRenderTargetView));
	RELEASE_COM(pBackBuffer);

	D3D11_TEXTURE2D_DESC dxDepthStencilDesc;
	dxDepthStencilDesc.Width = nWidth;
	dxDepthStencilDesc.Height = nHeight;
	dxDepthStencilDesc.MipLevels = 1;
	dxDepthStencilDesc.ArraySize = 1;
	dxDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dxDepthStencilDesc.SampleDesc.Count = 1;
	dxDepthStencilDesc.SampleDesc.Quality = 0;
	dxDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	dxDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dxDepthStencilDesc.CPUAccessFlags = 0;
	dxDepthStencilDesc.MiscFlags = 0;

	HR(m_pDevice->CreateTexture2D(&dxDepthStencilDesc, 0, &m_pDepthStencilBuffer));
	HR(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView));

	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	m_dxViewport.TopLeftX = 0;
	m_dxViewport.TopLeftY = 0;
	m_dxViewport.Width = static_cast<float>(nWidth);
	m_dxViewport.Height = static_cast<float>(nHeight);
	m_dxViewport.MinDepth = 0.f;
	m_dxViewport.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &m_dxViewport);

	DirectX::XMMATRIX mtProjection = DirectX::XMMatrixPerspectiveFovLH(DEG_TO_RAD(75), static_cast<float>(nWidth) / nHeight, 1.f, 1000.f);
	DirectX::XMStoreFloat4x4(&m_mtProjection, mtProjection);
}

bool CGraphicsContext::LoadShaders() {
	ID3DBlob* pVertexShaderBlob = nullptr;
	ID3DBlob* pPixelShaderBlob = nullptr;
	HR(D3DReadFileToBlob(L"game/shaders/vertex_shader_c.cso", &pVertexShaderBlob));
	HR(D3DReadFileToBlob(L"game/shaders/pixel_shader_c.cso", &pPixelShaderBlob));
	if (!pVertexShaderBlob || !pPixelShaderBlob)
		return false;

	// Initializing vertex shader
	HRESULT hResult = m_pDevice->CreateVertexShader(
		pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(),
		nullptr,
		&m_pVertexShader
	);

	if (FAILED(hResult)) {
		LOG_ERROR("Failed to create vertex shader.");
		RELEASE_COM(pVertexShaderBlob);
		return false;
	}

	// Initializing pixel shader
	hResult = m_pDevice->CreatePixelShader(
		pPixelShaderBlob->GetBufferPointer(),
		pPixelShaderBlob->GetBufferSize(),
		nullptr,
		&m_pPixelShader
	);

	if (FAILED(hResult)) {
		LOG_ERROR("Failed to create pixel shader.");
		RELEASE_COM(pVertexShaderBlob);
		RELEASE_COM(pPixelShaderBlob);
		return false;
	}

	// Initializing constant buffer
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
	cbDesc.CPUAccessFlags = 0;

	DirectX::XMFLOAT4X4 mtIdentity;
	DirectX::XMStoreFloat4x4(&mtIdentity, DirectX::XMMatrixIdentity());

	D3D11_SUBRESOURCE_DATA BufferInitData = {};
	BufferInitData.pSysMem = &mtIdentity;

	hResult = m_pDevice->CreateBuffer(&cbDesc, &BufferInitData, &m_pWorldViewProjectionBuffer);
	if (FAILED(hResult)) {
		LOG_ERROR("Failed to create contstant buffer.");
		return false;
	}

	BuildVertexLayout(pVertexShaderBlob);

	RELEASE_COM(pVertexShaderBlob);
	RELEASE_COM(pPixelShaderBlob);

	LOG_INFO("Successfully loaded shaders!");
	return true;
}

void CGraphicsContext::BuildVertexLayout(ID3DBlob* pVSBlob) {
	D3D11_INPUT_ELEMENT_DESC VertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HR(m_pDevice->CreateInputLayout(VertexDesc, 2, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout));
}
