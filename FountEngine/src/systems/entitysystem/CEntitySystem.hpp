#pragma once
#include <memory>
#include <array>
#include <vector>
#include "game/baseentity/IBaseEntity.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "game/baseentity/entityhandle/CEntityHandle.hpp"
#define ENT_MAX_ENGINE_ENTRY 16384

struct EntitySlot_t {
	std::unique_ptr<IBaseEntity> pEntity;
	uint32_t nSerial = 0;
	bool bUsed = false;

	EntitySlot_t* pNext = nullptr;
	EntitySlot_t* pPrev = nullptr;
};

class CEntitySystem {
public:
	void Initialize();

	// Create
	template<typename T = IBaseEntity, typename... Args>
	T* CreateEntity(Args&&... args);

	// Getters
	template<typename T>
	T* GetEntityByIndex(const uint32_t nIndex);
	template<typename T>
	T* GetEntityByHandle(const CEntityHandle Handle);
	
	EntitySlot_t* GetFirstSlot() const { return m_pActiveHeadSlot; }

	// Destroyers
	void DestroyEntityByHandle(const CEntityHandle Handle);

	// Other
	void UpdateAllEntities(float flDeltaTime);

private:
	bool m_bInitialized = false;
	EntitySlot_t* m_pActiveHeadSlot = nullptr;

	std::array<EntitySlot_t, ENT_MAX_ENGINE_ENTRY> m_arrEntitySlots;
	std::vector<uint32_t> m_vecFreeSlots;
};

template<typename T, typename ...Args>
inline T* CEntitySystem::CreateEntity(Args&& ...args) {
	static_assert(std::is_base_of<IBaseEntity, T>::value, "Template must derive from IBaseEntity");
	if (m_vecFreeSlots.empty()) {
		LOG_WARNING("Failed to create entity: no free slots");
		return nullptr;
	}

	uint32_t nIndex = m_vecFreeSlots.back();
	m_vecFreeSlots.pop_back();

	auto pEntity = std::make_unique<T>(std::forward<Args>(args)...);
	IBaseEntity* pBase = pEntity.get();

	EntitySlot_t& Slot = m_arrEntitySlots[nIndex];
	Slot.nSerial += 1;
	pBase->SetHandle(CEntityHandle(nIndex, Slot.nSerial));
	Slot.pEntity = std::move(pEntity);
	Slot.bUsed = true;

	Slot.pNext = m_pActiveHeadSlot;
	Slot.pPrev = nullptr;
	if (m_pActiveHeadSlot) m_pActiveHeadSlot->pPrev = &Slot;
	m_pActiveHeadSlot = &Slot;

	pBase->OnSpawn();
	return static_cast<T*>(pBase);
}

template<typename T>
inline T* CEntitySystem::GetEntityByIndex(const uint32_t nIndex) {
	static_assert(std::is_base_of<IBaseEntity, T>::value, "Template must derive from IBaseEntity");
	if (nIndex >= ENT_MAX_ENGINE_ENTRY) {
		LOG_WARNING("Failed to get entity by index: index too high.");
		return nullptr;
	}

	const EntitySlot_t& IndexSlot = m_arrEntitySlots[nIndex];
	if (!IndexSlot.bUsed) return nullptr;

	return static_cast<T*>(IndexSlot.pEntity.get());
}

template<typename T>
inline T* CEntitySystem::GetEntityByHandle(const CEntityHandle Handle) {
	static_assert(std::is_base_of<IBaseEntity, T>::value, "Template must derive from IBaseEntity");
	uint32_t nIndex = Handle.GetIndex();
	if (nIndex >= ENT_MAX_ENGINE_ENTRY) {
		LOG_WARNING("Failed to get entity by handle: index too high.");
		return nullptr;
	}

	uint32_t nSerial = Handle.GetSerial();
	const EntitySlot_t& HandleSlot = m_arrEntitySlots[nIndex];
	if (HandleSlot.nSerial != nSerial) {
		LOG_WARNING("Failed to get entity by handle: different serial numbers.");
		return nullptr;
	}

	return static_cast<T*>(HandleSlot.pEntity.get());
}
