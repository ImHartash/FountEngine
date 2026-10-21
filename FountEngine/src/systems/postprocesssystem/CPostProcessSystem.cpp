#include "CPostProcessSystem.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "game/effects/simpleeffect/CSimpleEffect.hpp"

bool CPostProcessSystem::Initialize(int nWidth, int nHeight, ID3D11Device* pDevice) {
	bool bResult = true;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	// Initializing system effects (tone mapping)
	bResult &= m_ToneMappingEffect.InitializeEffect(
		L"game/shaders/postprocess/tonemapping_c.cso", pDevice, nWidth, nHeight);

	// Initializing custom effects
	// Example: this->InitializeHDREffect<CBloomEffect>(L"game/shaders/postprocessing/bloom_c.cso", pDevice);
	// IMPORTANT: Initialize the effects in the order in which they should process the texture.

	// Initializing LDR
	//bResult &= this->InitializeLDREffect<CSimpleEffect>(
	//	L"game/shaders/postprocess/pixeling_c.cso", pDevice);

	bResult &= this->InitializeLDREffect<CSimpleEffect>(
		L"game/shaders/postprocess/chromatic_abberation_c.cso", pDevice);

	// Initializing Render Targets
	m_HDRTexture1.Initialize(pDevice, nWidth, nHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, RT_CREATE_FLAG_SRV | RT_CREATE_FLAG_UAV);
	m_HDRTexture2.Initialize(pDevice, nWidth, nHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, RT_CREATE_FLAG_SRV | RT_CREATE_FLAG_UAV);

	m_LDRTexture1.Initialize(pDevice, nWidth, nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, RT_CREATE_FLAG_SRV | RT_CREATE_FLAG_UAV);
	m_LDRTexture2.Initialize(pDevice, nWidth, nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, RT_CREATE_FLAG_SRV | RT_CREATE_FLAG_UAV);

	return bResult;
}

CDXRenderTarget* CPostProcessSystem::Process(CDXRenderTarget* pOffscreenTexture, ID3D11DeviceContext* pDeviceContext) {
	if (pOffscreenTexture == nullptr || pOffscreenTexture->GetWidth() != m_HDRTexture1.GetWidth()
		|| pOffscreenTexture->GetHeight() != m_HDRTexture1.GetHeight()) {
		LOG_WARNING("Failed to CPostProcessSystem::Process. Invalid offscreen texture.");
		return nullptr;
	}

	// Applying HDR post-processing
	for (uint8_t nEffectIndex = 0; nEffectIndex < static_cast<uint8_t>(m_vecHDREffects.size()); nEffectIndex++) {
		IPostEffect* pEffect = m_vecHDREffects[nEffectIndex].get();
		if (!pEffect->IsEnabled()) continue;
		if (nEffectIndex == 0) pEffect->ApplyEffect(pOffscreenTexture, &m_HDRTexture2, pDeviceContext);
		else if (nEffectIndex % 2 == 0) pEffect->ApplyEffect(&m_HDRTexture1, &m_HDRTexture2, pDeviceContext);
		else pEffect->ApplyEffect(&m_HDRTexture2, &m_HDRTexture1, pDeviceContext);
	}

	if (m_vecHDREffects.empty()) m_ToneMappingEffect.ApplyEffect(pOffscreenTexture, &m_LDRTexture1, pDeviceContext);
	else if (m_vecHDREffects.size() % 2 == 0) m_ToneMappingEffect.ApplyEffect(&m_HDRTexture1, &m_LDRTexture1, pDeviceContext);
	else m_ToneMappingEffect.ApplyEffect(&m_HDRTexture2, &m_LDRTexture1, pDeviceContext);

	// Applying LDR post-processing
	for (uint8_t nEffectIndex = 0; nEffectIndex < static_cast<uint8_t>(m_vecLDREffects.size()); nEffectIndex++) {
		IPostEffect* pEffect = m_vecLDREffects[nEffectIndex].get();
		if (!pEffect->IsEnabled()) continue;
		if (nEffectIndex % 2 == 0) pEffect->ApplyEffect(&m_LDRTexture1, &m_LDRTexture2, pDeviceContext);
		else pEffect->ApplyEffect(&m_LDRTexture2, &m_LDRTexture1, pDeviceContext);
	}

	return (m_vecLDREffects.size() % 2 == 0) ? &m_LDRTexture1 : &m_LDRTexture2;
}

void CPostProcessSystem::OnResize(int nWidth, int nHeight, ID3D11Device* pDevice) {
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_HDRTexture1.Resize(pDevice, nWidth, nHeight);
	m_HDRTexture2.Resize(pDevice, nWidth, nHeight);
	m_LDRTexture1.Resize(pDevice, nWidth, nHeight);
	m_LDRTexture2.Resize(pDevice, nWidth, nHeight);

	m_ToneMappingEffect.OnResize(nWidth, nHeight);

	for (uint8_t nEffectIndex = 0; nEffectIndex < static_cast<uint8_t>(m_vecHDREffects.size()); nEffectIndex++) {
		m_vecHDREffects[nEffectIndex]->OnResize(nWidth, nHeight);
	}

	for (uint8_t nEffectIndex = 0; nEffectIndex < static_cast<uint8_t>(m_vecLDREffects.size()); nEffectIndex++) {
		m_vecLDREffects[nEffectIndex]->OnResize(nWidth, nHeight);
	}
}