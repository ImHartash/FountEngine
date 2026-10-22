#pragma once
#include <d3d11.h>
#include <cstdint>

enum ERenderTargetCreateFlags : uint8_t {
	RT_CREATE_FLAG_RTV = (1 << 0),
	RT_CREATE_FLAG_SRV = (1 << 1),
	RT_CREATE_FLAG_UAV = (1 << 2)
};

class CDXRenderTarget {
public:
	CDXRenderTarget(
		ID3D11Device* pDevice,
		uint32_t nWidth,
		uint32_t nHeight,
		DXGI_FORMAT Format,
		uint8_t nCreateFlags);
	CDXRenderTarget();
	~CDXRenderTarget();

	CDXRenderTarget(const CDXRenderTarget&) = delete;
	CDXRenderTarget& operator=(const CDXRenderTarget&) = delete;

	void Initialize(ID3D11Device* pDevice,
		uint32_t nWidth,
		uint32_t nHeight,
		DXGI_FORMAT Format, 
		uint8_t nCreateFlags);
	void Resize(ID3D11Device* pDevice, uint32_t nWidth, uint32_t nHeight);

	ID3D11Texture2D* GetTexture() const { return m_pTexture; }
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }
	ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_pUnorderedAccessView; }

	uint32_t GetWidth() const { return m_nWidth; }
	uint32_t GetHeight() const { return m_nHeight; }

private:
	ID3D11Texture2D* m_pTexture;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11ShaderResourceView* m_pShaderResourceView;
	ID3D11UnorderedAccessView* m_pUnorderedAccessView;

	uint32_t m_nWidth;
	uint32_t m_nHeight;

	uint8_t m_nCreateFlags;
	DXGI_FORMAT m_nDXFormat;
};