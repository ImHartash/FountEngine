#include "postprocess_common.hlsl"

SamplerState gSampler : register(s0) {
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
    ComparisonFunc = NEVER;
    MaxLOD = 3.402823466e+38f; // FLT_MAX
};

static const float gCAStrength = 0.05f;
static const float gCAFalloff = 1.f;

[numthreads(8, 8, 1)]
void main(uint3 nDispatchThreadID : SV_DispatchThreadID)
{
    uint nWidth, nHeight;
    gOutputTexture.GetDimensions(nWidth, nHeight);
    if (nDispatchThreadID.x >= nWidth || nDispatchThreadID.y >= nHeight) 
        return;
    
    float2 UV = (float2(nDispatchThreadID.xy) + 0.5f) / float2(nWidth, nHeight);
    
    float2 vecDirection = UV - 0.5f;
    float flR2 = dot(vecDirection, vecDirection);
    float2 vecOffset = vecDirection * gCAStrength * pow(flR2, gCAFalloff * 0.5f);
    
    float2 UVR = saturate(UV + vecOffset);
    float2 UVB = saturate(UV - vecOffset);
    
    float r = gInputTexture.SampleLevel(gSampler, UVR, 0).r;
    float4 g = gInputTexture.SampleLevel(gSampler, UV, 0);
    float b = gInputTexture.SampleLevel(gSampler, UVB, 0).b;
    
    gOutputTexture[nDispatchThreadID.xy] = float4(r, g.g, b, g.a);
}