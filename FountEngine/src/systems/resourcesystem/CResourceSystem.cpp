#include "CResourceSystem.hpp"
#include "systems/filesystem/CFileSystem.hpp"
#include "systems/filesystem/headers/fntmdl_header.hpp"
#include "systems/filesystem/headers/fntmdl_vertex.hpp"
#include "systems/filesystem/headers/fnttex_header.hpp"
#include "systems/filesystem/headers/fntmat_header.hpp"
#include "engine/graphicscontext/CGraphicsContext.hpp"
#include "game/resource/modelresourcedata/CModelResourceData.hpp"
#include "game/resource/textureresourcedata/CTextureResourceData.hpp"
#include "game/resource/materialresourcedata/CMaterialResourceData.hpp"
#include "math/mathutils/CMathUtils.hpp"
#include "math/vertex.hpp"

//IResource* CResourceSystem::LoadResource(const std::string& strPath) {
//	uint32_t nMagic;
//	std::vector<char> vecFileData;
//	
//	if (!g_pFileSystem->ReadFile(strPath, vecFileData)) {
//		LOG_WARNING("Failed to load %s: resource file not found.", strPath.c_str());
//		return nullptr;
//	}
//
//	const char* pDataPointer = vecFileData.data();
//	memcpy(&nMagic, pDataPointer, sizeof(uint32_t));
//
//	if (nMagic == 0x4D544E46) { // FNTM (Model)
//		return this->LoadModel(vecFileData, strPath);
//	}
//	else if (nMagic == 0x54544E46) { // FNTT (Texture)
//		return this->LoadTexture(vecFileData, strPath);
//	}
//	else if (nMagic == 0x544D5446) { // FTMT (Material)
//		return this->LoadMaterial(vecFileData, strPath);
//	}
//
//	LOG_WARNING("Failed to load %s: file format is not supported.", strPath.c_str());
//	return nullptr;
//}

void CResourceSystem::Unload(const CResourceHandle& hResource) {
	if (!hResource.IsValid() || hResource.GetIndex() >= m_vecResourceCache.size())
		return;

	ResourceSlot_t& Slot = m_vecResourceCache[hResource.GetIndex()];
	if (!Slot.bUsed) return;
	Slot.nReferenceCount -= 1;

	if (Slot.nReferenceCount <= 0) {
		Slot.nReferenceCount = 0;
		Slot.pResource.reset();
		Slot.strResourcePath.clear();
		Slot.bUsed = false;

		m_mapPathToHandle.erase(Slot.strResourcePath);
		m_vecFreeSlots.push_back(hResource.GetIndex());
	}
}

void CResourceSystem::UnloadAll() {
	m_vecResourceCache.clear();
	m_vecFreeSlots.clear();
	m_mapPathToHandle.clear();
}

CResourceHandle CResourceSystem::LoadResource(const std::string& strPath) {
	// Checking resouce is loaded or not
	if (m_mapPathToHandle.find(strPath) != m_mapPathToHandle.end()) {
		CResourceHandle Handle = m_mapPathToHandle[strPath];
		ResourceSlot_t& Slot = m_vecResourceCache[Handle.GetIndex()];
		Slot.nReferenceCount += 1;
		return Handle;
	}

	uint32_t nMagic;
	std::vector<char> vecFileData;

	if (!g_pFileSystem->ReadFile(strPath, vecFileData)) {
		LOG_WARNING("Failed to load %s: resource file not found.", strPath.c_str());
		return CResourceHandle();
	}

	const char* pFileData = vecFileData.data();
	memcpy(&nMagic, pFileData, sizeof(uint32_t));

	std::unique_ptr<IResource> pResource;
	if (nMagic == 0x4D544E46) { // FNTM (Model)
		pResource = this->LoadModel(vecFileData, strPath);
	}
	else if (nMagic == 0x54544E46) { // FNTT (Texture)
		pResource = this->LoadTexture(vecFileData, strPath);
	}
	else if (nMagic == 0x544D5446) { // FTMT (Material)
		pResource = this->LoadMaterial(vecFileData, strPath);
	}
	else {
		LOG_WARNING("Failed to load %s: invalid or not supported file format.", strPath.c_str());
		return CResourceHandle();
	}

	if (!pResource) {
		LOG_WARNING("Failed to load %s: resource loading failed.", strPath.c_str());
		return CResourceHandle();
	}

	ResourceSlot_t ResourceSlot;
	ResourceSlot.pResource = std::move(pResource);
	ResourceSlot.strResourcePath = strPath;
	ResourceSlot.nReferenceCount = 1;
	ResourceSlot.bUsed = true;

	uint32_t nIndex;
	if (!m_vecFreeSlots.empty()) {
		nIndex = m_vecFreeSlots.back();
		m_vecFreeSlots.pop_back();
		m_vecResourceCache[nIndex] = std::move(ResourceSlot);
	}
	else {
		nIndex = m_vecResourceCache.size();
		m_vecResourceCache.push_back(std::move(ResourceSlot));
	}

	m_mapPathToHandle[strPath] = CResourceHandle(nIndex);
	LOG_INFO("Loaded resource: %s", strPath.c_str());
	return CResourceHandle(nIndex);
}

std::unique_ptr<IResource> CResourceSystem::LoadModel(std::vector<char>& vecDataBuffer, const std::string& strResourceName) {
	const char* pBufferPointer = vecDataBuffer.data();

	FNTMDL_HEADER Header = {};
	memcpy(&Header, pBufferPointer, sizeof(FNTMDL_HEADER));

	if (Header.nVersion != SDK_VERSION) {
		LOG_WARNING("Failed to load %s: version is outdated.", strResourceName.c_str());
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

	std::unique_ptr<CModelResourceData> pResource = std::make_unique<CModelResourceData>(
		std::move(vecVertices), std::move(vecIndicesData)
	);
	LOG_INFO("Successfully pre-cached %s.", strResourceName.c_str());
	return pResource;
}

std::unique_ptr<IResource> CResourceSystem::LoadTexture(std::vector<char>& vecDataBuffer, const std::string& strResourceName) {
	const char* pBufferPointer = vecDataBuffer.data();

	FNTTEX_HEADER Header = {};
	memcpy(&Header, pBufferPointer, sizeof(FNTTEX_HEADER));
	if (Header.nVersion != SDK_VERSION) {
		LOG_WARNING("Failed to load %s: version is outdated.", strResourceName.c_str());
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
		LOG_WARNING("Failed to load %s: DX function `CreateShaderResourceView` failed.", strResourceName.c_str());
		return nullptr;
	}

	std::unique_ptr<CTextureResourceData> pResource = std::make_unique<CTextureResourceData>(SRV);
	LOG_INFO("Successfully pre-cached %s.", strResourceName.c_str());
	return pResource;
}

std::unique_ptr<IResource> CResourceSystem::LoadMaterial(std::vector<char>& vecDataBuffer, const std::string& strResourceName) {
	const char* pBufferPointer = vecDataBuffer.data();

	FNTMAT_HEADER Header = {};
	memcpy(&Header, pBufferPointer, sizeof(FNTMAT_HEADER));
	if (Header.nVersion != SDK_VERSION) {
		LOG_WARNING("Failed to load %s: version is outdated.", strResourceName.c_str());
		return nullptr;
	}

	FNTMAT_DATA MaterialData = {};
	memcpy(&MaterialData, pBufferPointer + sizeof(FNTMAT_HEADER), sizeof(FNTMAT_DATA));

	std::string strDiffuseTexture = std::string(MaterialData.szDiffuseTexture);
	CResourceHandle hDiffuseTexture;
	if (m_mapPathToHandle.find(strDiffuseTexture) == m_mapPathToHandle.end()) {
		hDiffuseTexture = this->LoadResource(strDiffuseTexture);
	}
	else {
		hDiffuseTexture = m_mapPathToHandle[strDiffuseTexture];
	}

	std::unique_ptr<CMaterialResourceData> pResource = std::make_unique<CMaterialResourceData>(
		hDiffuseTexture,
		Vector3_t(MaterialData.flAmbient), Vector3_t(MaterialData.flDiffuse), Vector3_t(MaterialData.flSpecular),
		MaterialData.flShininess, MaterialData.flOpacity,
		static_cast<EMaterialBlendMode>(MaterialData.nBlendMode),
		static_cast<EMaterialCullMode>(MaterialData.nCullMode),
		static_cast<EMaterialDepthMode>(MaterialData.nDepthMode)
	);
	LOG_INFO("Successfully pre-cached %s. %.2f", strResourceName.c_str(), MaterialData.flOpacity);
	return pResource;
}