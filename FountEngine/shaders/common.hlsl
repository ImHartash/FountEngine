cbuffer MatrixBuffer : register(b0)
{
    matrix WorldViewProjectionMatrix;
}

struct VertexIn
{
    float3 flPosition : POSITION;
    float4 flColor : COLOR;
};

struct VertexOut
{
    float4 flPosition : SV_POSITION;
    float4 flColor : COLOR;
};