#pragma once
#include <DirectXMath.h>

struct Vertex_t {
	DirectX::XMFLOAT3 vec3Position;
	DirectX::XMFLOAT3 vec3Normal;
	DirectX::XMFLOAT2 vec2Texcoord;
};