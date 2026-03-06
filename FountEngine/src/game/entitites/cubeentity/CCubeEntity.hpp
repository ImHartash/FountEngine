#pragma once
#include "../basemodelentity/CBaseModelEntity.hpp"

class CCubeEntity : public CBaseModelEntity {
public:
	void OnSpawn() override;
	void Update(float flDeltaTime) override;
};