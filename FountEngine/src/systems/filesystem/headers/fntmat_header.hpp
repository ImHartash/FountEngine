#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct FNTMAT_HEADER {
	uint32_t nMagic;
	uint32_t nVersion;
};

struct FNTMAT_DATA {
	uint32_t nBlendMode;
	uint32_t nCullMode;
	uint32_t nDepthMode;

	float flAmbient[3];
	float flDiffuse[3];
	float flSpecular[3];

	float flShininess;
	float flOpacity;

	char szDiffuseTexture[260];
};
#pragma pack(pop)