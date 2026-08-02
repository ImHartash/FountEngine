#include "common.hlsl"

// TODO: Make TexTransform and WorldInvTranspose for buffer.
VertexOut_t FntVertexShader(VertexIn_t VertexIn)
{
    VertexOut_t VertexOut;
    
    float4 flVertexPosition = float4(VertexIn.vPosition, 1.0f);
    flVertexPosition = mul(flVertexPosition, gWorldViewProjectionMatrix);
    VertexOut.vPosition = flVertexPosition;
    VertexOut.vNormal = VertexIn.vNormal;
    VertexOut.vTexcoord = VertexIn.vTexcoord;
    
    return VertexOut;
}