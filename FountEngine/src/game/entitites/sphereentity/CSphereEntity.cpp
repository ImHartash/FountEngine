#include "CSphereEntity.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"

void CSphereEntity::OnSpawn() {
	LOG_INFO("Spawning Test Sphere.");
	this->SetMaterialResource("materials/test_light.fntmat");
	this->SetModelResource("models/sphere.fntmdl");
}

void CSphereEntity::Update(float flDeltaTime) {
	// m_angViewAngle += Angle_t(0.f, 0.5f, 0.f) * flDeltaTime;
}
