#pragma once
#include "../basemodelentity/CBaseModelEntity.hpp"

class CSpaceObject : public CBaseModelEntity {
public:
	CSpaceObject(CBaseModelEntity* pCenterEntity, float flOrbitRadius, float flOrbitSpeed,
		float flSpinSpeed) : m_pCenterEntity(pCenterEntity), m_flOrbitRadius(flOrbitRadius), 
		m_flOrbitSpeed(flOrbitSpeed), m_flSpinSpeed(flSpinSpeed) { };

	void OnSpawn() override {};
	void Update(float flDeltaTime) override;

private:
	float m_flOrbitAngle = 0.f;

	CBaseModelEntity* m_pCenterEntity;
	float m_flOrbitRadius;
	float m_flOrbitSpeed;
	float m_flSpinSpeed;
};