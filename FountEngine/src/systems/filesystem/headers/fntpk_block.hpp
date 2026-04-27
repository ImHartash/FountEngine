#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct FNTPK_BLOCK {
	char szResourceName[256];
	uint32_t nBlockDataOffset;
	uint32_t nBlockDataSize;
};
#pragma pack(pop)