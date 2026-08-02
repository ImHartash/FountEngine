#include "light_shader.hlsl"

cbuffer cbPerObject : register(b0)
{
    matrix gWorldViewProjectionMatrix;
    Material_t gMaterial;
}

cbuffer cbLightBuffer : register(b1)
{
    DirectionalLight_t gDirectionalLights[MAX_DIR_LIGHTS];
    PointLight_t gPointLights[MAX_POINT_LIGHTS];
    SpotLight_t gSpotLights[MAX_SPOT_LIGHTS];
    
    
}

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn_t
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL; // Unused for any time
    float2 vTexcoord : TEXCOORD;
};

struct VertexOut_t
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL; // Unused for any time
    float2 vTexcoord : TEXCOORD;
};