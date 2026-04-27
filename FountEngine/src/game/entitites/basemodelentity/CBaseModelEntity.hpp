#pragma once
#include <string>
#include "game/baseentity/IBaseEntity.hpp"
#include "math/types/vector3.hpp"
#include "math/types/angle.hpp"

class CBaseModelEntity : public IBaseEntity {
public:
	void Update(float flDeltaTime) override {};
	void OnSpawn() override {};
	void OnDestroy() override {};

	std::string GetModelResource() const { return m_strModelResource; }
	void SetModelResource(const std::string& strModelResource) { m_strModelResource = strModelResource; }

	std::string GetTextureResource() const { return m_strTextureResource; }
	void SetTextureResource(const std::string& strTextureResource) { m_strTextureResource = strTextureResource; }

	Vector3_t& GetPosition() { return m_vecPosition; }
	Angle_t& GetViewAngle() { return m_angViewAngle; }

	void SetPosition(const Vector3_t& vecPosition) { m_vecPosition = vecPosition; }
	void SetRotation(const Angle_t& angViewAngle) { m_angViewAngle = angViewAngle; }

protected:
	std::string m_strModelResource;
	std::string m_strTextureResource;

	Vector3_t m_vecPosition;
	Angle_t m_angViewAngle;
};