#pragma once
#include <string>
#include "../IResource.hpp"
#include "enums/material.hpp"
#include "math/types/vector3.hpp"

class CMaterialResourceData : public IResource {
public:
	CMaterialResourceData() = default;
	CMaterialResourceData(
		std::string strDiffuseTexture,
		const Vector3_t& vecAmbient,
		const Vector3_t& vecDiffuse,
		const Vector3_t& vecSpecular,
		float flShininess,
		float flOpacity,
		EMaterialBlendMode BlendMode,
		EMaterialCullMode CullMode,
		EMaterialDepthMode DepthMode
	)
		: m_strDiffuseTexture(std::move(strDiffuseTexture)),
		m_vecAmbient(vecAmbient), m_vecDiffuse(vecDiffuse), m_vecSpecular(vecSpecular),
		m_flShininess(flShininess), m_flOpacity(flOpacity),
		m_BlendMode(BlendMode), m_CullMode(CullMode), m_DepthMode(DepthMode) {
	}

	const std::string& GetDiffuseTexture() const { return m_strDiffuseTexture; }

	const Vector3_t& GetAmbient() const { return m_vecAmbient; }
	const Vector3_t& GetDiffuse() const { return m_vecDiffuse; }
	const Vector3_t& GetSpecular() const { return m_vecSpecular; }

	float GetShininess() const { return m_flShininess; }
	float GetOpacity() const { return m_flOpacity; }

	EMaterialBlendMode GetBlendMode() const { return m_BlendMode; }
	EMaterialCullMode GetCullMode() const { return m_CullMode; }
	EMaterialDepthMode GetDepthMode() const { return m_DepthMode; }

private:
	std::string m_strDiffuseTexture;

	Vector3_t m_vecAmbient;
	Vector3_t m_vecDiffuse;
	Vector3_t m_vecSpecular;

	float m_flShininess;
	float m_flOpacity;

	EMaterialBlendMode m_BlendMode;
	EMaterialCullMode m_CullMode;
	EMaterialDepthMode m_DepthMode;
};