#include "CEntitySystem.hpp"

CEntitySystem& CEntitySystem::GetInstance() {
	static CEntitySystem Instance;
	return Instance;
}

void CEntitySystem::UpdateAllEntities(float flDeltaTime) {
	for (const auto& pEntity : m_vecEntityList) {
		pEntity->Update(flDeltaTime);
	}
}
