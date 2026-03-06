#include "CResourceSystem.hpp"
#include <fstream>
#include "systems/filesystem/CFileSystem.hpp"
#include "systems/logging/CLogSystem.hpp"
#include "headers/fntmdl_header.hpp"
#include "headers/fntmdl_vertex.hpp"
#include "engine/renderer/CRenderer.hpp"

#define FNTMDL_VERSION 1

CResourceSystem& CResourceSystem::GetInstance() {
	static CResourceSystem Instance;
	return Instance;
}

CModelResourceData* CResourceSystem::GetModelResourceData(const std::string& strModelPath) {
	auto Iterator = m_vecModelsCache.find(strModelPath);
	if (Iterator != m_vecModelsCache.end())
		return &Iterator->second;

	return this->LoadResource(strModelPath);
}

CModelResourceData* CResourceSystem::LoadResource(const std::string& strResourcePath) {
	std::string strExtension = CFileSystem::GetInstance().GetExtension(strResourcePath);
	if (strExtension != ".fntmdl") {
		LOG_ERROR("Unsupported resource file. (%s)", strResourcePath.c_str());
		return nullptr;
	}

	std::string strRelativePath = CFileSystem::GetInstance().GetRelativePath(strResourcePath);
	std::ifstream fModelBinary(strRelativePath, std::ios::binary);
	if (!fModelBinary.is_open()) {
		LOG_INFO("Failed to load %s: file not found or couldn't be opened.", strResourcePath.c_str());
		return nullptr;
	}

	FNTMDL_HEADER Header{};
	fModelBinary.read(reinterpret_cast<char*>(&Header), sizeof(FNTMDL_HEADER));
	if (Header.nMagic != 0x464E544D) {
		LOG_ERROR("Failed to load % s: file have invalid header(%d != %d).",
			strResourcePath.c_str(), Header.nMagic, 0x464E544D);
		return nullptr;
	}

	if (Header.nVersion != FNTMDL_VERSION) {
		LOG_ERROR("Failed to load % s: file have invalid version(%d != %d).",
			strResourcePath.c_str(), Header.nVersion, FNTMDL_VERSION);
		return nullptr;
	}

	std::vector<_FNTMDL_VERTEX> vecFileVertices(Header.nVertexCount);
	std::vector<uint32_t> vecFileIndices(Header.nIndexCount);

	fModelBinary.seekg(Header.nVertexOffset);
	fModelBinary.read(reinterpret_cast<char*>(vecFileVertices.data()),
		Header.nVertexCount * sizeof(_FNTMDL_VERTEX));

	fModelBinary.seekg(Header.nIndexOffset);
	fModelBinary.read(reinterpret_cast<char*>(vecFileIndices.data()),
		Header.nIndexCount * sizeof(uint32_t));

	std::vector<Vertex_t> vecVertices;
	vecVertices.reserve(Header.nVertexCount);
	for (const auto& vertex : vecFileVertices) {
		Vertex_t Vertex;
		Vertex.vec3Position = { vertex.px, vertex.py, vertex.pz };
		Vertex.vec4Color = { .5f, .5f, .5f, 1.f };
		vecVertices.push_back(Vertex);
	}

	LOG_DEBUG("Pre-Caching resource %s...", strResourcePath.c_str());

	m_vecModelsCache.emplace(
		strResourcePath,
		CModelResourceData(std::move(vecVertices), std::move(vecFileIndices))
	);
	CModelResourceData& ModelResourceData = m_vecModelsCache[strResourcePath];

	CRenderer::GetInstance().AddToStaticBuffers(&ModelResourceData);

	return &ModelResourceData;
}
