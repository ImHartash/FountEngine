cbuffer MatrixBuffer : register(b0)
{
    matrix WorldViewProjectionMatrix;
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