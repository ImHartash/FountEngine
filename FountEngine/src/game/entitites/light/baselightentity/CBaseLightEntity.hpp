#pragma once
#include "game/baseentity/IBaseEntity.hpp"
#include "math/types/vector3.hpp"

class CBaseLightEntity : public IBaseEntity {
public:
	CBaseLightEntity() = default;
	CBaseLightEntity(const Vector3_t& vecAmbient, const Vector3_t& vecDiffuse, const Vector3_t& vecSpecular)
		: m_vecAmbient(vecAmbient), m_vecDiffuse(vecDiffuse), m_vecSpecular(vecSpecular) { }

	void Update(float flDeltaTime) override {};
	void OnSpawn() override {};
	void OnDestroy() override {};

	Vector3_t GetAmbient() const { return m_vecAmbient; }
	void SetAmbient(const Vector3_t& vecAmbient) { m_vecAmbient = vecAmbient; }

	Vector3_t GetDiffuse() const { return m_vecDiffuse; }
	void SetDiffuse(const Vector3_t& vecDiffuse) { m_vecDiffuse = vecDiffuse; }

	Vector3_t GetSpecular() const { return m_vecSpecular; }
	void SetSpecular(const Vector3_t& vecSpecular) { m_vecSpecular = vecSpecular; }

protected:
	uint32_t GetFlags() const override;

	Vector3_t m_vecAmbient;
	Vector3_t m_vecDiffuse;
	Vector3_t m_vecSpecular;
};