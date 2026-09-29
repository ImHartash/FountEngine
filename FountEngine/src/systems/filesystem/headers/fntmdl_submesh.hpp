#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct FNTMDL_SUBMESH {
	uint32_t nIndexOffset;
	uint32_t nIndexCount;
	char szMaterialPath[260];
};
#pragma pack(pop)