#pragma once
#include <cstdint>

struct CModelBufferInfo {
	uint32_t nVertexOffset;
	uint32_t nIndexOffset;

	uint32_t nVertexCount;
	uint32_t nIndexCount;
};