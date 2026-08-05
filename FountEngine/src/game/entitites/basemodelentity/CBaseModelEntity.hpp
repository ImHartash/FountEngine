#pragma once
#include <string>
#include "game/resource/CResourceHandle.hpp"
#include "game/baseentity/IBaseEntity.hpp"
#include "math/types/vector3.hpp"
#include "math/types/angle.hpp"

class CBaseModelEntity : public IBaseEntity {
public:
	void Update(float flDeltaTime) override {};
	void OnSpawn() override {};
	void OnDestroy() override {};

	CResourceHandle GetModelResource() const { return m_hModelResource; }
	void SetModelResource(const std::string& strModelResource);

	CResourceHandle GetMaterialResource() const { return m_hMaterialResource; }
	void SetMaterialResource(const std::string& strMaterialResource);

	Vector3_t& GetPosition() { return m_vecPosition; }
	Angle_t& GetViewAngle() { return m_angViewAngle; }

	void SetPosition(const Vector3_t& vecPosition) { m_vecPosition = vecPosition; }
	void SetRotation(const Angle_t& angViewAngle) { m_angViewAngle = angViewAngle; }

protected:
	uint32_t GetFlags() const override;

	CResourceHandle m_hModelResource;
	CResourceHandle m_hMaterialResource;

	Vector3_t m_vecPosition;
	Angle_t m_angViewAngle;
};