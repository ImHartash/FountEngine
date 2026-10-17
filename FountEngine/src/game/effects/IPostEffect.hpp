#pragma once
#include <string>
#include <d3d11.h>
#include <cstdint>

class CDXRenderTarget;

class IPostEffect {
public:
	virtual ~IPostEffect() = default;

	virtual bool InitializeEffect(const std::wstring& strShaderFilename, ID3D11Device* pDevice,
		int nWidth, int nHeight) = 0;

	virtual void ApplyEffect(CDXRenderTarget* pInput, CDXRenderTarget* pOutput, 
		ID3D11DeviceContext* pDeviceContext) = 0;
	virtual void OnResize(uint32_t nWidth, uint32_t nHeight) { this->m_nWidth = nWidth; this->m_nHeight = nHeight; }

protected:
	uint32_t m_nWidth;
	uint32_t m_nHeight;
	ID3D11ComputeShader* m_pEffectShader = nullptr;
};