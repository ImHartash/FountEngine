#pragma once
#include "../baselightentity/CBaseLightEntity.hpp"

class CSpotLight : public CBaseLightEntity {
public:
	CSpotLight() = default;
	CSpotLight(const Vector3_t& vecAmbient, const Vector3_t& vecDiffuse, const Vector3_t& vecSpecular,
		const Vector3_t& vecPosition, const Vector3_t& vecAttenuation, const Vector3_t& vecDirection,
		const float flRange, const float flSpot)
		: CBaseLightEntity(vecAmbient, vecDiffuse, vecSpecular), m_vecPosition(vecPosition),
		m_vecAttenuation(vecAttenuation), m_vecDirection(vecDirection), 
		m_flRange(flRange), m_flSpot(flSpot) { };

	Vector3_t GetPosition() const { return m_vecPosition; }
	void SetPosition(const Vector3_t& vecPosition) { m_vecPosition = vecPosition; }

	Vector3_t GetAttenuation() const { return m_vecAttenuation; }
	void SetAttenuation(const Vector3_t& vecAttenuation) { m_vecAttenuation = vecAttenuation; }

	Vector3_t GetDirection() const { return m_vecDirection; }
	void SetDirection(const Vector3_t& vecDirection) { m_vecDirection = vecDirection; }

	float GetRange() const { return m_flRange; }
	void SetRange(const float flRange) { m_flRange = flRange; }

	float GetSpot() const { return m_flSpot; }
	void SetSpot(const float flSpot) { m_flSpot = flSpot; }

protected:
	uint32_t GetFlags() const override;

	Vector3_t m_vecPosition;
	Vector3_t m_vecAttenuation;
	Vector3_t m_vecDirection;
	float m_flRange;
	float m_flSpot;
};