#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "game/resource/CResourceHandle.hpp"
#include "game/resource/IResource.hpp"

#define SDK_VERSION 1

struct ResourceSlot_t {
	std::unique_ptr<IResource> pResource;
	std::string strResourcePath;
	int nReferenceCount;
	bool bUsed;
};

class CResourceSystem {
public:
	CResourceSystem() = default;
	~CResourceSystem() = default;

	template<typename T = IResource>
	T* GetResourceByName(const std::string& strResourceName);
	template<typename T = IResource>
	T* GetResource(const CResourceHandle hResource);

	void Unload(const CResourceHandle& hResource);
	void UnloadAll();

	CResourceHandle LoadResource(const std::string& strPath);

private:
	std::unique_ptr<IResource> LoadModel(std::vector<char>& vecDataBuffer, const std::string& strResourceName);
	std::unique_ptr<IResource> LoadTexture(std::vector<char>& vecDataBuffer, const std::string& strResourceName);
	std::unique_ptr<IResource> LoadMaterial(std::vector<char>& vecDataBuffer, const std::string& strResourceName);

	std::vector<ResourceSlot_t> m_vecResourceCache;
	std::unordered_map<std::string, CResourceHandle> m_mapPathToHandle;
	std::vector<uint32_t> m_vecFreeSlots;
};

template<typename T>
inline T* CResourceSystem::GetResourceByName(const std::string& strResourceName) {
	if (m_mapPathToHandle.find(strResourceName) == m_mapPathToHandle.end()) {
		LOG_WARNING("Failed to get resource %s: resource not loaded.");
		return nullptr;
	}

	CResourceHandle hResource = m_mapPathToHandle[strResourceName];
	return this->GetResource<T>(hResource);
}

template<typename T>
inline T* CResourceSystem::GetResource(const CResourceHandle hResource) {
	if (!hResource.IsValid() || hResource.GetIndex() >= m_vecResourceCache.size()) {
		LOG_WARNING("Failed to get resource %s: resource handle is not valid.");
		return nullptr;
	}

	const ResourceSlot_t& Slot = m_vecResourceCache[hResource.GetIndex()];
	if (!Slot.bUsed) {
		LOG_WARNING("Failed to get resource %s: resource slot is not in using.");
		return nullptr;
	}

	return dynamic_cast<T*>(Slot.pResource.get());
}
