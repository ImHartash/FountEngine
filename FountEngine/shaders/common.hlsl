#include "light_shader.hlsl"

cbuffer cbPerObject : register(b0)
{
    matrix gWorldMatrix;
    matrix gWorldInverseTranspose;
    matrix gWorldViewProjectionMatrix;
    Material_t gMaterial;
}

cbuffer cbLightBuffer : register(b1)
{
    DirectionalLight_t gDirectionalLights[MAX_DIR_LIGHTS];
    PointLight_t gPointLights[MAX_POINT_LIGHTS];
    SpotLight_t gSpotLights[MAX_SPOT_LIGHTS];
    FogData_t gFogData;
    
    float3 gCameraPosition;
    float _flPad0;
    
    uint gDirectionalLightsNumber;
    uint gPointLightsNumber;
    uint gSpotLightsNumber;
    uint _nPad0;
}

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn_t
{
    float3 vPosition : POSITION;
    float3 vNormalLocal : NORMAL; // Unused for any time
    float2 vTexcoord : TEXCOORD;
};

struct VertexOut_t
{   
    float4 vPosition : SV_POSITION;
    float3 vPositionWorld : POSITION;
    float3 vNormalWorld : NORMAL; // Unused for any time
    float2 vTexcoord : TEXCOORD;
};