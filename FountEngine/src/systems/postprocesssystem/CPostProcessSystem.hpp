#pragma once
#include <vector>
#include <memory>
#include <type_traits>
#include "game/effects/IPostEffect.hpp"
#include "game/effects/tonemapping/CToneMappingEffect.hpp"
#include "CDXRenderTarget/CDXRenderTarget.hpp"

class CPostProcessSystem {
public:
	bool Initialize(int nWidth, int nHeight, ID3D11Device* pDevice);

	CDXRenderTarget* Process(CDXRenderTarget* pOffscreenTexture, ID3D11DeviceContext* pDeviceContext);
	void OnResize(int nWidth, int nHeight, ID3D11Device* pDevice);

	template<typename T>
	bool InitializeHDREffect(const std::wstring& wstrShaderFile, ID3D11Device* pDevice);
	template<typename T>
	bool InitializeLDREffect(const std::wstring& wstrShaderFile, ID3D11Device* pDevice);

private:
	CDXRenderTarget m_HDRTexture1;
	CDXRenderTarget m_HDRTexture2;

	CDXRenderTarget m_LDRTexture1;
	CDXRenderTarget m_LDRTexture2;

	std::vector<std::unique_ptr<IPostEffect>> m_vecHDREffects;
	std::vector<std::unique_ptr<IPostEffect>> m_vecLDREffects;

	CToneMappingEffect m_ToneMappingEffect;

	uint32_t m_nWidth;
	uint32_t m_nHeight;
};

template<typename T>
inline bool CPostProcessSystem::InitializeHDREffect(const std::wstring& wstrShaderFile, ID3D11Device* pDevice) {
	static_assert(std::is_base_of<IPostEffect, T>::value, "Template must derive from IPostEffect");

	std::unique_ptr<T> pEffect = std::make_unique<T>();
	if (!pEffect->InitializeEffect(wstrShaderFile, pDevice, m_nWidth, m_nHeight))
		return false;

	m_vecHDREffects.push_back(std::move(pEffect));
	return true;
}

template<typename T>
inline bool CPostProcessSystem::InitializeLDREffect(const std::wstring& wstrShaderFile, ID3D11Device* pDevice) {
	static_assert(std::is_base_of<IPostEffect, T>::value, "Template must derive from IPostEffect");

	std::unique_ptr<T> pEffect = std::make_unique<T>();
	if (!pEffect->InitializeEffect(wstrShaderFile, pDevice, m_nWidth, m_nHeight))
		return false;

	m_vecLDREffects.push_back(std::move(pEffect));
	return true;
}