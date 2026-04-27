#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct FNTTEX_HEADER {
	uint32_t nMagic; // 'FNTT'
	uint32_t nVersion;
	uint32_t nWidth;
	uint32_t nHeight;
	uint32_t nMipLevels;
	uint32_t nDXFormat;
};
#pragma pack(pop)