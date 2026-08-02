#include "common.hlsl"

float4 FntPixelShader(VertexOut_t pIn) : SV_TARGET
{
    float4 vPixelColor = gTexture.Sample(gSampler, pIn.vTexcoord);
    vPixelColor.a *= gMaterial.flOpacity;
    
    return vPixelColor;
}