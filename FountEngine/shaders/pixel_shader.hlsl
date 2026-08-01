#include "common.hlsl"

float4 FntPixelShader(VertexOut pIn) : SV_TARGET
{
    float4 fl4PixelColor = gTexture.Sample(gSampler, pIn.flTexcoord);
    fl4PixelColor.a *= gMaterial.flMaterialOpacity;
    
    return fl4PixelColor;
}