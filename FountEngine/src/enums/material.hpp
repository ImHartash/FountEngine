#pragma once
#include <cstdint>

enum class EMaterialBlendMode : uint32_t {
	Opaque = 0,
	AlphaBlend,
	Additive
};

enum class EMaterialCullMode : uint32_t {
	Back = 0,
	Front,
	None
};

enum class EMaterialDepthMode : uint32_t {
	Enabled = 0,
	Disabled,
	ReadOnly
};