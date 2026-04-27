#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct FNTMDL_HEADER {
    uint32_t nMagic; // 'MTNF'
    uint32_t nVersion;
    uint32_t nVertexCount;
    uint32_t nIndexCount;
    uint32_t nVertexOffset;
    uint32_t nIndexOffset;
    uint32_t nFlags;
};
#pragma pack(pop)