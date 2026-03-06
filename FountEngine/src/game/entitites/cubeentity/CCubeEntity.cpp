#include "CCubeEntity.hpp"
#include "systems/logging/CLogSystem.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"

void CCubeEntity::OnSpawn() {
	LOG_INFO("Spawning Test Cube.");
	CResourceSystem::GetInstance().LoadResource("simple_cube_test_c.fntmdl");
	this->SetModelResource("simple_cube_test_c.fntmdl");
}

void CCubeEntity::Update(float flDeltaTime) {
	m_angViewAngle += Angle_t(0.f, 0.5f, 0.f) * flDeltaTime;
}
