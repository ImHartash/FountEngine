#include "CDXRenderTarget.hpp"
#include "utils/defines.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"

CDXRenderTarget::CDXRenderTarget(ID3D11Device* pDevice, uint32_t nWidth,
	uint32_t nHeight, DXGI_FORMAT Format, uint8_t nCreateFlags) :
	m_pTexture(nullptr), m_pRenderTargetView(nullptr),
	m_pShaderResourceView(nullptr), m_pUnorderedAccessView(nullptr)
{
	this->Initialize(pDevice, nWidth, nHeight, Format, nCreateFlags);
}

CDXRenderTarget::CDXRenderTarget() : m_pTexture(nullptr), m_pRenderTargetView(nullptr),
	m_pShaderResourceView(nullptr), m_pUnorderedAccessView(nullptr), 
	m_nHeight(0), m_nWidth(0), m_nCreateFlags(0), m_nDXFormat(DXGI_FORMAT_UNKNOWN) { }

CDXRenderTarget::~CDXRenderTarget() {
	RELEASE_COM(m_pUnorderedAccessView);
	RELEASE_COM(m_pShaderResourceView);
	RELEASE_COM(m_pRenderTargetView);
	RELEASE_COM(m_pTexture);
}

void CDXRenderTarget::Initialize(ID3D11Device* pDevice, uint32_t nWidth,
	uint32_t nHeight, DXGI_FORMAT Format, uint8_t nCreateFlags) 
{
	RELEASE_COM(m_pUnorderedAccessView);
	RELEASE_COM(m_pShaderResourceView);
	RELEASE_COM(m_pRenderTargetView);
	RELEASE_COM(m_pTexture);

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_nCreateFlags = nCreateFlags;
	m_nDXFormat = Format;

	uint32_t nDescFlags = 0;
	if (nCreateFlags & RT_CREATE_FLAG_RTV) nDescFlags |= D3D11_BIND_RENDER_TARGET;
	if (nCreateFlags & RT_CREATE_FLAG_SRV) nDescFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (nCreateFlags & RT_CREATE_FLAG_UAV) nDescFlags |= D3D11_BIND_UNORDERED_ACCESS;

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = nWidth;
	TextureDesc.Height = nHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = Format;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = nDescFlags;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	HR(pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture));
	if (nCreateFlags & RT_CREATE_FLAG_RTV) HR(pDevice->CreateRenderTargetView(m_pTexture, nullptr, &m_pRenderTargetView));
	if (nCreateFlags & RT_CREATE_FLAG_SRV) HR(pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pShaderResourceView));
	if (nCreateFlags & RT_CREATE_FLAG_UAV) HR(pDevice->CreateUnorderedAccessView(m_pTexture, nullptr, &m_pUnorderedAccessView));
}

void CDXRenderTarget::Resize(ID3D11Device* pDevice, uint32_t nWidth, uint32_t nHeight) {
	this->Initialize(pDevice, nWidth, nHeight, m_nDXFormat, m_nCreateFlags);
}