#include "CCubeEntity.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"

void CCubeEntity::OnSpawn() {
	LOG_INFO("Spawning Test Cube.");
	this->SetMaterialResource("materials/test_cube.fntmat");
	this->SetModelResource("models/test_cube.fntmdl");
}

void CCubeEntity::Update(float flDeltaTime) {
	m_angViewAngle += Angle_t(0.f, 0.5f, 0.f) * flDeltaTime;
}
