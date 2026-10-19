#include "CSimpleEffect.hpp"
#include "systems/postprocesssystem/CDXRenderTarget/CDXRenderTarget.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "utils/defines.hpp"
#include <d3dcompiler.h>
#include <cmath>

bool CSimpleEffect::InitializeEffect(const std::wstring& strShaderFilename, ID3D11Device* pDevice,
	int nWidth, int nHeight) {
	m_nWidth = static_cast<uint32_t>(nWidth);
	m_nHeight = static_cast<uint32_t>(nHeight);

	ID3DBlob* pShaderBlob = nullptr;
	HRESULT hResult = D3DReadFileToBlob(strShaderFilename.c_str(), &pShaderBlob);
	if (FAILED(hResult)) {
		LOG_WARNING("Failed to initialize effect [D3DReadFileToBlob]");
		RELEASE_COM(pShaderBlob);
		return false;
	}

	hResult = pDevice->CreateComputeShader(
		pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(),
		nullptr,
		&m_pEffectShader
	);
	if (FAILED(hResult)) {
		LOG_WARNING("Failed to initialize effect [CreateComputeShader]");
		RELEASE_COM(pShaderBlob);
		RELEASE_COM(m_pEffectShader);
		return false;
	}

	RELEASE_COM(pShaderBlob);
	return true;
}

void CSimpleEffect::ApplyEffect(CDXRenderTarget* pInput, CDXRenderTarget* pOutput,
	ID3D11DeviceContext* pDeviceContext)
{
	if (pInput == nullptr || pOutput == nullptr || pDeviceContext == nullptr || pInput == pOutput) return;
	pDeviceContext->CSSetShader(m_pEffectShader, nullptr, 0);

	ID3D11ShaderResourceView* pShaderResourceView = pInput->GetShaderResourceView();
	ID3D11UnorderedAccessView* pUnorderedAccessView = pOutput->GetUnorderedAccessView();
	pDeviceContext->CSSetShaderResources(0, 1, &pShaderResourceView);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, &pUnorderedAccessView, nullptr);

	UINT nNumThreadsX = (pInput->GetWidth() + 7) / 8;
	UINT nNumThreadsY = (pInput->GetHeight() + 7) / 8;

	pDeviceContext->Dispatch(nNumThreadsX, nNumThreadsY, 1);

	// Clearing
	ID3D11ShaderResourceView* pClearShaderResourceView = { nullptr };
	ID3D11UnorderedAccessView* pClearUnorderedAccessView = { nullptr };
	pDeviceContext->CSSetShaderResources(0, 1, &pClearShaderResourceView);
	pDeviceContext->CSSetUnorderedAccessViews(0, 1, &pClearUnorderedAccessView, nullptr);

	pDeviceContext->CSSetShader(nullptr, nullptr, 0);
}

CSimpleEffect::~CSimpleEffect() {
	RELEASE_COM(m_pEffectShader);
}