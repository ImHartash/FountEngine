#pragma once
#include <DirectXMath.h>
#define NUM_MAX_DIR_LIGHTS   4
#define NUM_MAX_POINT_LIGHTS 8
#define NUM_MAX_SPOT_LIGHTS  4

struct DirectionalLightData_t {
	DirectX::XMFLOAT3 vec3Ambient;
	float _flPad0;
	DirectX::XMFLOAT3 vec3Diffuse;
	float _flPad1;
	DirectX::XMFLOAT3 vec3Specular;
	float _flPad2;

	DirectX::XMFLOAT3 vec3Direction;
	float _flPad3;
};

struct PointLightData_t
{
    DirectX::XMFLOAT3 vec3Ambient;
    float _flPad0;
    DirectX::XMFLOAT3 vec3Diffuse;
    float _flPad1;
    DirectX::XMFLOAT3 vec3Specular;
    float _flPad2;

    DirectX::XMFLOAT3 vec3Position;
    float flRange;

    DirectX::XMFLOAT3 vec3Attenuation;
    float _flPad3;
};

struct SpotLightData_t
{
    DirectX::XMFLOAT3 vec3Ambient;
    float _flPad0;
    DirectX::XMFLOAT3 vec3Diffuse;
    float _flPad1;
    DirectX::XMFLOAT3 vec3Specular;
    float _flPad2;

    DirectX::XMFLOAT3 vec3Position;
    float flRange;

    DirectX::XMFLOAT3 vec3Direction;
    float flSpot;

    DirectX::XMFLOAT3 vec3Attenuation;
    float _flPad3;
};

struct FogData_t {
    uint32_t nFogEnabled; // 0 = disabled, other = enabled (please, use 1, it's more like boolean)
    float flFogStart;
    float flFogRange;
    float _flPad0;

    DirectX::XMFLOAT3 vec3FogColor;
    float _flPad1;
};

struct LightBuffer_t {
    DirectionalLightData_t arrDirectionalLights[NUM_MAX_DIR_LIGHTS];
    PointLightData_t arrPointLights[NUM_MAX_POINT_LIGHTS];
    SpotLightData_t arrSpotLights[NUM_MAX_SPOT_LIGHTS];
    FogData_t FogData;

    DirectX::XMFLOAT3 vec3CameraPositionWorld;
    float _flPad0;

    uint32_t nDirectionalLightsNumber;
    uint32_t nPointLightsNumber;
    uint32_t nSpotLightsNumber;
    uint32_t _nPad0;
};