#include "CBaseModelEntity.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"

void CBaseModelEntity::SetModelResource(const std::string& strModelResource) {
	m_hModelResource = g_pResourceSystem->LoadResource(strModelResource);
}

void CBaseModelEntity::SetMaterialResource(const std::string& strMaterialResource) {
	m_hMaterialResource = g_pResourceSystem->LoadResource(strMaterialResource);
}
