#include "common.hlsl"

VertexOut FntVertexShader(VertexIn vIn)
{
    VertexOut vOut;
    
    float4 flVertexPosition = float4(vIn.flPosition, 1.0f);
    flVertexPosition = mul(flVertexPosition, WorldViewProjectionMatrix);
    vOut.flPosition = flVertexPosition;
    vOut.flColor = vIn.flColor;
    
    return vOut;
}