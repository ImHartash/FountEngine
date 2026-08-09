#pragma once
#include "../baselightentity/CBaseLightEntity.hpp"

class CPointLight : public CBaseLightEntity {
public:
	CPointLight() = default;
	CPointLight(const Vector3_t& vecAmbient, const Vector3_t& vecDiffuse, const Vector3_t& vecSpecular,
		const Vector3_t& vecPosition, const Vector3_t& vecAttenuation, const float flRange)
		: CBaseLightEntity(vecAmbient, vecDiffuse, vecSpecular), m_vecPosition(vecPosition), 
		m_vecAttenuation(vecAttenuation), m_flRange(flRange) { };

	Vector3_t GetPosition() const { return m_vecPosition; }
	void SetPosition(const Vector3_t& vecPosition) { m_vecPosition = vecPosition; }

	Vector3_t GetAttenuation() const { return m_vecAttenuation; }
	void SetAttenuation(const Vector3_t& vecAttenuation) { m_vecAttenuation = vecAttenuation; }

	float GetRange() const { return m_flRange; }
	void SetRange(const float flRange) { m_flRange = flRange; }

protected:
	uint32_t GetFlags() const override;

	Vector3_t m_vecPosition;
	Vector3_t m_vecAttenuation;
	float m_flRange;
};