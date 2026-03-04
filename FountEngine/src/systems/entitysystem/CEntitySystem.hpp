#pragma once
#include <memory>
#include <vector>

#include "game/baseentity/IBaseEntity.hpp"
#include "systems/logging/CLogSystem.hpp"

class CEntitySystem {
public:
	template<typename T = IBaseEntity, typename... Args>
	T* CreateEntity(Args&&... args);

	template<typename T>
	T* GetEntityByIndex(const uint32_t nIndex);

	void UpdateAllEntities(float flDeltaTime);

private:
	std::vector<std::unique_ptr<IBaseEntity>> m_vecEntityList;
	uint32_t m_nMaxIndex = 1;
};

template<typename T, typename ...Args>
inline T* CEntitySystem::CreateEntity(Args && ...args) {
	static_assert(std::is_base_of<IBaseEntity, T>::value, "Template must derive from IBaseEntity");
	auto pEntityPtr = std::make_unique<T>(std::forward<Args>(args)...);

	IBaseEntity* pEntity = pEntityPtr.get();
	pEntity->SetIndex(m_nMaxIndex++);
	pEntity->OnSpawn();

	m_vecEntityList.emplace_back(std::move(pEntityPtr));

	return pEntity;
}

template<typename T>
inline T* CEntitySystem::GetEntityByIndex(const uint32_t nIndex) {
	if (nIndex >= m_vecEntityList.size()) {
		LOG_WARNING("Index %d is beyond maximum EntityList limits.", nIndex);
		return nullptr;
	}

	return dynamic_cast<T*>(m_vecEntityList[nIndex].get());
}
