#include "CBaseModelEntity.hpp"
#include "systems/filesystem/CFileSystem.hpp"

CBaseModelEntity::CBaseModelEntity(const std::string& strModelResource) {
	m_strModelResource = strModelResource;
	CFileSystem::GetInstance().LoadResource(strModelResource);
}
