#include "common.hlsl"

// TODO: Make TexTransform and WorldInvTranspose for buffer.
VertexOut FntVertexShader(VertexIn vIn)
{
    VertexOut vOut;
    
    float4 flVertexPosition = float4(vIn.flPosition, 1.0f);
    flVertexPosition = mul(flVertexPosition, WorldViewProjectionMatrix);
    vOut.flPosition = flVertexPosition;
    vOut.flNormal = vIn.flNormal;
    vOut.flTexcoord = vIn.flTexcoord;
    
    return vOut;
}