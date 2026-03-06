#pragma once
#include <unordered_map>
#include <string>
#include "game/resource/modelresourcedata/CModelResourceData.hpp"

class CResourceSystem {
public:
	static CResourceSystem& GetInstance();

	CModelResourceData* GetModelResourceData(const std::string& strModelPath);
	CModelResourceData* LoadResource(const std::string& strModelPath);

private:
	std::unordered_map<std::string, CModelResourceData> m_vecModelsCache;
};