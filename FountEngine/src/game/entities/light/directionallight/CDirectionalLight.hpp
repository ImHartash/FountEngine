#pragma once
#include "../baselightentity/CBaseLightEntity.hpp"

class CDirectionalLight : public CBaseLightEntity {
public:
	CDirectionalLight() = default;
	CDirectionalLight(const Vector3_t& vecAmbient, const Vector3_t& vecDiffuse, const Vector3_t& vecSpecular,
		const Vector3_t& vecDirection)
		: CBaseLightEntity(vecAmbient, vecDiffuse, vecSpecular), m_vecDirection(vecDirection) { };

	Vector3_t GetDirection() const { return m_vecDirection; }
	void SetDirection(const Vector3_t& vecDirection) { m_vecDirection = vecDirection; }

protected:
	uint32_t GetFlags() const override;

	Vector3_t m_vecDirection;
};