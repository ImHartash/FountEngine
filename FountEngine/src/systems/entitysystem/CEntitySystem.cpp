#include "CEntitySystem.hpp"

void CEntitySystem::Initialize() {
	m_vecFreeSlots.reserve(ENT_MAX_ENGINE_ENTRY);
	for (uint32_t nIndex = ENT_MAX_ENGINE_ENTRY; nIndex-- > 0;) {
		m_vecFreeSlots.push_back(nIndex);
	}
}

void CEntitySystem::DestroyEntityByHandle(const CEntityHandle Handle) {
	uint32_t nIndex = Handle.GetIndex();
	if (nIndex >= ENT_MAX_ENGINE_ENTRY) {
		LOG_WARNING("DestroyEntityByHandle: index %d out of range.", nIndex);
		return;
	}

	EntitySlot_t& HandleSlot = m_arrEntitySlots[nIndex];
	if (!HandleSlot.bUsed) {
		LOG_WARNING("DestroyEntityByHandle: slot %d is already free.", nIndex);
		return;
	}

	if (HandleSlot.nSerial != Handle.GetSerial()) {
		LOG_WARNING("DestroyEntityByHandle: serial mismatch for slot %d.", nIndex);
		return;
	}

	if (HandleSlot.pPrev) HandleSlot.pPrev->pNext = HandleSlot.pNext;
	else m_pActiveHeadSlot = HandleSlot.pNext;
	if (HandleSlot.pNext) HandleSlot.pNext->pPrev = HandleSlot.pPrev;

	HandleSlot.pNext = nullptr;
	HandleSlot.pPrev = nullptr;
	HandleSlot.pEntity.reset();
	HandleSlot.bUsed = false;

	m_vecFreeSlots.push_back(nIndex);
}

void CEntitySystem::UpdateAllEntities(float flDeltaTime) {
	for (EntitySlot_t* pSlot = m_pActiveHeadSlot; pSlot != nullptr; pSlot = pSlot->pNext) {
		if (pSlot->pEntity) pSlot->pEntity->Update(flDeltaTime);
	}
}