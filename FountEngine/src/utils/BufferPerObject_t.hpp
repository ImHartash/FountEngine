#pragma once
#include <DirectXMath.h>

struct MaterialData_t {
	DirectX::XMFLOAT3 vec3Ambient;
	float flShininess;

	DirectX::XMFLOAT3 vec3Diffuse;
	float flOpacity;

	DirectX::XMFLOAT3 vec3Specular;
	float _flPad0;
};

struct BufferPerObject_t {
	DirectX::XMFLOAT4X4 mtWorldViewProjection;
	MaterialData_t Material;
};