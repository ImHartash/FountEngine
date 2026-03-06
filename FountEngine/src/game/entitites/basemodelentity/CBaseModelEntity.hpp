#pragma once
#include <string>
#include "game/baseentity/IBaseEntity.hpp"
#include "math/types/vector3.hpp"
#include "math/types/angle.hpp"

class CBaseModelEntity : public IBaseEntity {
public:
	CBaseModelEntity(const std::string& strModelResource);

	void Update(float flDeltaTime) override {};
	void OnSpawn() override {};
	void OnDestroy() override {};

	std::string GetModelResource() const { return m_strModelResource; }
	void SetModelResource(const std::string& strModelResource) { m_strModelResource = strModelResource; }

private:
	std::string m_strModelResource;

	Vector3_t m_vecPosition;
	Angle_t m_angRotation;
};