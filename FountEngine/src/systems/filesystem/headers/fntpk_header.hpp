#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct FNTPK_HEADER {
	uint32_t nMagic; // 'FNTP'
	uint32_t nVersion;
	uint32_t nFlags;
	uint32_t nCRC;
	uint32_t nBlocksCount;
};
#pragma pack(pop)