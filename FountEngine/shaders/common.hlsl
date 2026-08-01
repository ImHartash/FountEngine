struct Material
{
    float3 fl3MaterialAmbient;
    float flMaterialShininess;
    
    float3 fl3MaterialDiffuse;
    float flMaterialOpacity;
    
    float3 fl3MaterialSpecular;
    float _flPad0;
};

cbuffer cbPerObject : register(b0)
{
    matrix gWorldViewProjectionMatrix;
    Material gMaterial;
}

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct VertexIn
{
    float3 flPosition : POSITION;
    float3 flNormal : NORMAL; // Unused for any time
    float2 flTexcoord : TEXCOORD;
};

struct VertexOut
{
    float4 flPosition : SV_POSITION;
    float3 flNormal : NORMAL; // Unused for any time
    float2 flTexcoord : TEXCOORD;
};