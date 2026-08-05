#pragma once
#include "../basemodelentity/CBaseModelEntity.hpp"

class CSphereEntity : public CBaseModelEntity {
public:
	void OnSpawn() override;
	void Update(float flDeltaTime) override;
};