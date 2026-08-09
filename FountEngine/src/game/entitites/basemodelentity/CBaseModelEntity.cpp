#include "CBaseModelEntity.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"
#include "enums/entity_flags.hpp"

void CBaseModelEntity::SetModelResource(const std::string& strModelResource) {
	m_hModelResource = g_pResourceSystem->LoadResource(strModelResource);
}

void CBaseModelEntity::SetMaterialResource(const std::string& strMaterialResource) {
	m_hMaterialResource = g_pResourceSystem->LoadResource(strMaterialResource);
}

uint32_t CBaseModelEntity::GetFlags() const {
	return ENT_FLAG_TYPE_RENDER_ENTITY;
}
