#include "CMaterialResourceData.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"

CMaterialResourceData::~CMaterialResourceData() {
	g_pResourceSystem->Unload(m_hDiffuseTexture);
}
