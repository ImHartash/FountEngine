#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "game/resource/IResource.hpp"

#define SDK_VERSION 1

class CResourceSystem {
public:
	static CResourceSystem& GetInstance();

	template<typename T>
	T* GetResource(const std::string& strResourceName);
	IResource* LoadResource(const std::string& strPath);

private:
	IResource* LoadModel(std::vector<char>& vecDataBuffer, const std::string& strResourceName);
	IResource* LoadTexture(std::vector<char>& vecDataBuffer, const std::string& strResourceName);

	std::unordered_map<std::string, std::unique_ptr<IResource>> m_Cache;
};

template<typename T>
inline T* CResourceSystem::GetResource(const std::string& strResourceName) {
	auto It = m_Cache.find(strResourceName);
	if (It != m_Cache.end()) {
		return dynamic_cast<T*>(It->second.get());
	}
	return dynamic_cast<T*>(this->LoadResource(strResourceName));
}
