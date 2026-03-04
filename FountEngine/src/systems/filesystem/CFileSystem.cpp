#include "CFileSystem.hpp"
#include <filesystem>
#include <fstream>
#include "systems/logging/CLogSystem.hpp"
#include "headers/fntmdl_header.hpp"

CFileSystem& CFileSystem::GetInstance() {
	static CFileSystem Instance;
	return Instance;
}

void CFileSystem::Initialize() {
	std::filesystem::create_directory("game/resources");
	m_strResourcePath = "game/resources/";
}

bool CFileSystem::LoadResource(const std::string& strModelPath) {
	std::ifstream fModelBinary(m_strResourcePath + strModelPath, std::ios::binary);
	if (!fModelBinary.is_open()) {
		LOG_ERROR("Failed to load %s: file not found or couldn't be opened.", strModelPath.c_str());
		return false;
	}

	FNTMDL_HEADER Header{};
	fModelBinary.read(reinterpret_cast<char*>(&Header), sizeof(FNTMDL_HEADER));
	if (Header.nMagic != 0x464E544D) {
		LOG_ERROR("Failed to load %s: file have invalid header (%d != %d).", 
			strModelPath.c_str(), Header.nMagic, 0x464E544D);
		return false;
	}

	std::vector<_FNTMDL_VERTEX> vecFileVertices(Header.nVertexCount);
	std::vector<uint32_t> vecFileIndices(Header.nIndexCount);

	fModelBinary.seekg(Header.nVertexOffset);
	fModelBinary.read(reinterpret_cast<char*>(vecFileVertices.data()),
		Header.nVertexCount * sizeof(_FNTMDL_VERTEX));

	fModelBinary.seekg(Header.nIndexCount);
	fModelBinary.read(reinterpret_cast<char*>(vecFileIndices.data()),
		Header.nIndexCount * sizeof(uint32_t));

	// agh moment :(((
	std::vector<Vertex_t> vecVertices(Header.nVertexCount);
	for (const auto& vertex : vecFileVertices) {
		Vertex_t Vertex;
		Vertex.vec3Position = { vertex.px, vertex.py, vertex.pz };
		Vertex.vec4Color = { .5f, .5f, .5f, 1.f };
		vecVertices.push_back(Vertex);
	}

	m_vecModelsCache.emplace(strModelPath,
		CModelResourceData(std::move(vecVertices), std::move(vecFileIndices)));

	return true;
}

const CModelResourceData* CFileSystem::GetModel(const std::string& strModelPath) {
	return nullptr;
}
