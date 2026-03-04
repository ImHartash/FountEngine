#pragma once
#include <string>
#include <unordered_map>
#include "game/resource/modelresourcedata/CModelResourceData.hpp"

// TODO: Make simple filesystem for FountEngine

class CFileSystem {
public:
	static CFileSystem& GetInstance();

	void Initialize();
	bool LoadResource(const std::string& strModelPath);

	const CModelResourceData* GetModel(const std::string& strModelPath);

private:
	struct _FNTMDL_VERTEX {
		float px, py, pz;
		float nx, ny, nz;
		float tu, tv;
	};

	std::string m_strResourcePath;
	std::unordered_map<std::string, CModelResourceData> m_vecModelsCache;

	CFileSystem() = default;
	~CFileSystem() = default;
};