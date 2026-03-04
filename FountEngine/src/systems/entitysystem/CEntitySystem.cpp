#include "CEntitySystem.hpp"

void CEntitySystem::UpdateAllEntities(float flDeltaTime) {
	for (const auto& pEntity : m_vecEntityList) {
		pEntity->Update(flDeltaTime);
	}
}
