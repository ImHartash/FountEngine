#pragma once
#include "../IPostEffect.hpp"

class CToneMappingEffect : public IPostEffect {
public:
	bool InitializeEffect(const std::wstring& strShaderFilename, ID3D11Device* pDevice,
		int nWidth, int nHeight) override;

	void ApplyEffect(CDXRenderTarget* pInput, CDXRenderTarget* pOutput, 
		ID3D11DeviceContext* pDeviceContext) override;

	~CToneMappingEffect();
};