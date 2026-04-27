#pragma once
#include <d3d11.h>
#include "utils/defines.hpp"
#include "../IResource.hpp"

class CTextureResourceData : public IResource {
public:
	CTextureResourceData() 
		: m_pShaderResourceView(nullptr) { }
	CTextureResourceData(ID3D11ShaderResourceView* pSRV)
		: m_pShaderResourceView(pSRV) { }

	~CTextureResourceData() {
		RELEASE_COM(m_pShaderResourceView);
	}

	ID3D11ShaderResourceView* GetResourceView() const { return m_pShaderResourceView; }
	void SetShaderResourceView(ID3D11ShaderResourceView* pSRV) { m_pShaderResourceView = pSRV; }

private:
	ID3D11ShaderResourceView* m_pShaderResourceView;
};