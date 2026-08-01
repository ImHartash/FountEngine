#include "CGraphicsContext.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "systems/entitysystem/CEntitySystem.hpp"
#include "engine/renderer/CRenderer.hpp"
#include "math/defines.hpp"
#include "utils/defines.hpp"

CGraphicsContext& CGraphicsContext::GetInstance() {
	static CGraphicsContext Instance;
	return Instance;
}

bool CGraphicsContext::Initialize(HWND hWnd, int nWidth, int nHeight) {
	UINT uCreateDeviceFlags = 0;
#ifdef _DEBUG
	uCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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

	D3D_FEATURE_LEVEL FeatureLevel;
	HRESULT hResult = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		uCreateDeviceFlags,
		nullptr, 0,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwapChain,
		&m_pDevice,
		&FeatureLevel,
		&m_pDeviceContext
	);

	if (FAILED(hResult)) {
		LOG_ERROR("Failed to create ID3D11Device.");
		return false;
	}

	if (FeatureLevel < D3D_FEATURE_LEVEL_11_0) {
		LOG_ERROR("Direct3D feature level 11 unsupported.");
		return false;
	}

	CreateRenderStates();
	OnResize(nWidth, nHeight);
	return true;
}

void CGraphicsContext::Update(float flDeltaTime) {
	CRenderer::GetInstance().UpdateSceneComponents(flDeltaTime);
}

void CGraphicsContext::Render() {
	assert(m_pDeviceContext);
	assert(m_pSwapChain);

	const float aClearColor[4] = { 0.2f, 0.4f, 0.7f, 1.0f };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, aClearColor);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	CRenderer::GetInstance().PrepareFrame();
	CRenderer::GetInstance().RenderScene();

	HR(m_pSwapChain->Present(0, 0));
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

void CGraphicsContext::ApplyMaterialStates(EMaterialBlendMode BlendMode, 
	EMaterialCullMode CullMode, EMaterialDepthMode DepthMode) 
{
	const float arrBlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_pDeviceContext->OMSetBlendState(m_pBlendStates[static_cast<int32_t>(BlendMode)], arrBlendFactor, 0xFFFFFFFF);
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStates[static_cast<int32_t>(DepthMode)], 0);
	m_pDeviceContext->RSSetState(m_pRasterStates[static_cast<int32_t>(CullMode)]);
}

DirectX::XMFLOAT4X4& CGraphicsContext::GetProjectionMatrix() {
	return m_mtProjection;
}

CGraphicsContext::~CGraphicsContext() {
	RELEASE_COM(m_pDepthStencilBuffer);
	RELEASE_COM(m_pDepthStencilView);
	RELEASE_COM(m_pRenderTargetView);

	RELEASE_COM(m_pSwapChain);
	RELEASE_COM(m_pDeviceContext);
	RELEASE_COM(m_pDevice);

	for (int i = 0; i < 3; i++) {
		RELEASE_COM(m_pBlendStates[i]);
		RELEASE_COM(m_pDepthStates[i]);
		RELEASE_COM(m_pRasterStates[i]);
	}
}

void CGraphicsContext::CreateRenderStates() {
	// --- Blend states ---
	D3D11_BLEND_DESC OpaqueDesc = {};
	OpaqueDesc.RenderTarget[0].BlendEnable = false;
	OpaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(m_pDevice->CreateBlendState(&OpaqueDesc, &m_pBlendStates[static_cast<int>(EMaterialBlendMode::Opaque)]));

	D3D11_BLEND_DESC AlphaDesc = {};
	AlphaDesc.RenderTarget[0].BlendEnable = true;
	AlphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	AlphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	AlphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	AlphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	AlphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	AlphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	AlphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(m_pDevice->CreateBlendState(&AlphaDesc, &m_pBlendStates[static_cast<int>(EMaterialBlendMode::AlphaBlend)]));

	D3D11_BLEND_DESC AddDesc = {};
	AddDesc.RenderTarget[0].BlendEnable = true;
	AddDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	AddDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	AddDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	AddDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	AddDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	AddDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	AddDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(m_pDevice->CreateBlendState(&AddDesc, &m_pBlendStates[static_cast<int>(EMaterialBlendMode::Additive)]));

	// --- Depth states ---
	D3D11_DEPTH_STENCIL_DESC EnabledDesc = {};
	EnabledDesc.DepthEnable = true;
	EnabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	EnabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
	HR(m_pDevice->CreateDepthStencilState(&EnabledDesc, &m_pDepthStates[static_cast<int>(EMaterialDepthMode::Enabled)]));

	D3D11_DEPTH_STENCIL_DESC ReadOnlyDesc = EnabledDesc;
	ReadOnlyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	HR(m_pDevice->CreateDepthStencilState(&ReadOnlyDesc, &m_pDepthStates[static_cast<int>(EMaterialDepthMode::ReadOnly)]));

	D3D11_DEPTH_STENCIL_DESC DisabledDesc = {};
	DisabledDesc.DepthEnable = false;
	HR(m_pDevice->CreateDepthStencilState(&DisabledDesc, &m_pDepthStates[static_cast<int>(EMaterialDepthMode::Disabled)]));

	// --- Rasterizer states (cull modes) ---
	D3D11_RASTERIZER_DESC BackDesc = {};
	BackDesc.FillMode = D3D11_FILL_SOLID;
	BackDesc.CullMode = D3D11_CULL_BACK;
	HR(m_pDevice->CreateRasterizerState(&BackDesc, &m_pRasterStates[static_cast<int>(EMaterialCullMode::Back)]));

	D3D11_RASTERIZER_DESC FrontDesc = BackDesc;
	FrontDesc.CullMode = D3D11_CULL_FRONT;
	HR(m_pDevice->CreateRasterizerState(&FrontDesc, &m_pRasterStates[static_cast<int>(EMaterialCullMode::Front)]));

	D3D11_RASTERIZER_DESC NoneDesc = BackDesc;
	NoneDesc.CullMode = D3D11_CULL_NONE;
	HR(m_pDevice->CreateRasterizerState(&NoneDesc, &m_pRasterStates[static_cast<int>(EMaterialCullMode::None)]));
}