#include "CResourceSystem.hpp"
#include "systems/filesystem/CFileSystem.hpp"
#include "systems/filesystem/headers/fntmdl_header.hpp"
#include "systems/filesystem/headers/fntmdl_vertex.hpp"
#include "systems/filesystem/headers/fnttex_header.hpp"
#include "systems/logging/CLogSystem.hpp"
#include "engine/graphicscontext/CGraphicsContext.hpp"
#include "game/resource/modelresourcedata/CModelResourceData.hpp"
#include "game/resource/textureresourcedata/CTextureResourceData.hpp"
#include "math/mathutils/CMathUtils.hpp"
#include "math/vertex.hpp"

CResourceSystem& CResourceSystem::GetInstance() {
	static CResourceSystem Instance;
	return Instance;
}

IResource* CResourceSystem::LoadResource(const std::string& strPath) {
	uint32_t nMagic;
	std::vector<char> vecFileData;
	
	if (!CFileSystem::GetInstance().ReadFile(strPath, vecFileData)) {
		LOG_WARNING("Failed to load resource: resource file not found.");
		return nullptr;
	}

	const char* pDataPointer = vecFileData.data();
	memcpy(&nMagic, pDataPointer, sizeof(uint32_t));

	if (nMagic == 0x464E544D) {
		return this->LoadModel(vecFileData, strPath);
	}
	else if (nMagic == 0x54544E46) {
		return this->LoadTexture(vecFileData, strPath);
	}

	LOG_WARNING("Failed to load resource: file format is not supported.");
	return nullptr;
}

IResource* CResourceSystem::LoadModel(std::vector<char>& vecDataBuffer, const std::string& strResourceName) {
	const char* pBufferPointer = vecDataBuffer.data();

	FNTMDL_HEADER Header = {};
	memcpy(&Header, pBufferPointer, sizeof(FNTMDL_HEADER));

	if (Header.nVersion != SDK_VERSION) {
		LOG_WARNING("Failed to load model: outdated model version.");
		return nullptr;
	}

	std::vector<_FNTMDL_VERTEX> vecVerticesData(Header.nVertexCount);
	std::vector<uint32_t> vecIndicesData(Header.nIndexCount);

	memcpy(vecVerticesData.data(), pBufferPointer + Header.nVertexOffset, vecVerticesData.size() * sizeof(_FNTMDL_VERTEX));
	memcpy(vecIndicesData.data(), pBufferPointer + Header.nIndexOffset, vecIndicesData.size() * sizeof(uint32_t));

	std::vector<Vertex_t> vecVertices;
	vecVertices.reserve(vecVerticesData.size());

	for (auto& Vertex : vecVerticesData) {
		Vertex_t FntVertex;
		FntVertex.vec3Position = { Vertex.px, Vertex.py, Vertex.pz };
		FntVertex.vec3Normal = { Vertex.nx, Vertex.ny, Vertex.nz };
		FntVertex.vec2Texcoord = { Vertex.tu, Vertex.tv };

		vecVertices.push_back(FntVertex);
	}

	std::unique_ptr<CModelResourceData> pResourceData = std::make_unique<CModelResourceData>(
		std::move(vecVertices), std::move(vecIndicesData)
	);

	CModelResourceData* pResource = pResourceData.get();
	m_Cache[strResourceName] = std::move(pResourceData);
	LOG_INFO("Successfully pre-cached model.");
	return pResource;
}

IResource* CResourceSystem::LoadTexture(std::vector<char>& vecDataBuffer, const std::string& strResourceName) {
	const char* pBufferPointer = vecDataBuffer.data();

	FNTTEX_HEADER Header = {};
	memcpy(&Header, pBufferPointer, sizeof(FNTTEX_HEADER));
	if (Header.nVersion != SDK_VERSION) {
		LOG_WARNING("Failed to load texture: version is outdated.");
		return nullptr;
	}

	D3D11_TEXTURE2D_DESC TexDesc = {};
	TexDesc.Width = Header.nWidth;
	TexDesc.Height = Header.nHeight;
	TexDesc.MipLevels = Header.nMipLevels;
	TexDesc.ArraySize = 1;
	TexDesc.Format = static_cast<DXGI_FORMAT>(Header.nDXFormat);
	TexDesc.SampleDesc.Count = 1;
	TexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	std::vector<D3D11_SUBRESOURCE_DATA> vecInitData(Header.nMipLevels);
	const char* pPixelData = pBufferPointer + sizeof(FNTTEX_HEADER);
	size_t nOffset = 0;

	for (uint32_t i = 0; i < Header.nMipLevels; i++) {
		uint32_t nMipWidth = CMathUtils::Max(1u, Header.nWidth >> i);
		uint32_t nMipHeight = CMathUtils::Max(1u, Header.nHeight >> i);
		size_t nMipSize = static_cast<size_t>(nMipWidth) * nMipHeight * 4;

		vecInitData[i].pSysMem = pPixelData + nOffset;
		vecInitData[i].SysMemPitch = nMipWidth * 4;
		vecInitData[i].SysMemSlicePitch = 0;

		nOffset += nMipSize;
	}

	ID3D11Texture2D* pTexture = nullptr;
	HRESULT hr = CGraphicsContext::GetInstance().GetDevice()->CreateTexture2D(&TexDesc, vecInitData.data(), &pTexture);
	if (FAILED(hr)) return nullptr;

	ID3D11ShaderResourceView* SRV = nullptr;
	hr = CGraphicsContext::GetInstance().GetDevice()->CreateShaderResourceView(pTexture, nullptr, &SRV);
	pTexture->Release();

	if (FAILED(hr)) {
		if (pTexture) pTexture->Release();
		return nullptr;
	}

	std::unique_ptr<CTextureResourceData> pResourceData = std::make_unique<CTextureResourceData>(SRV);
	CTextureResourceData* pResource = pResourceData.get();
	m_Cache[strResourceName] = std::move(pResourceData);
	LOG_INFO("Successfully pre-cached texture.");
	return pResource;
}
