#include "common.hlsl"

float4 FntPixelShader(VertexOut pIn) : SV_TARGET
{
    return pIn.flColor;
}