#include "common.hlsl"

// TODO: Make TexTransform and WorldInvTranspose for buffer.
VertexOut_t FntVertexShader(VertexIn_t VertexIn)
{
    VertexOut_t VertexOut;
    
    VertexOut.vPositionWorld = mul(float4(VertexIn.vPosition, 1.0f), gWorldMatrix).xyz;
    VertexOut.vNormalWorld = mul(VertexIn.vNormalLocal, (float3x3) gWorldInverseTranspose);
    VertexOut.vPosition = mul(float4(VertexIn.vPosition, 1.0f), gWorldViewProjectionMatrix);
    VertexOut.vTexcoord = VertexIn.vTexcoord;
    
    return VertexOut;
}