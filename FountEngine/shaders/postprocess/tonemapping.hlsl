#include "postprocess_common.hlsl"

static const float3x3 ACESInputMat =
{
    { 0.59719, 0.35458, 0.04823 },
    { 0.07600, 0.90834, 0.01566 },
    { 0.02840, 0.13383, 0.83777 }
};

static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367 },
    { -0.10208, 1.10813, -0.00605 },
    { -0.00327, -0.07276, 1.07602 }
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 ACESFitted(float3 color)
{
    color = mul(ACESInputMat, color);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    // Clamp to [0, 1]
    color = saturate(color);

    return color;
}

// The code was taken from https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl

[numthreads(8, 8, 1)]
void main( uint3 nDispatchThreadID : SV_DispatchThreadID ) {
    uint nWidth, nHeight;
    gOutputTexture.GetDimensions(nWidth, nHeight);
    if (nDispatchThreadID.x >= nWidth || nDispatchThreadID.y >= nHeight) 
        return;
    
    float4 vecColor = gInputTexture.Load(int3(nDispatchThreadID.xy, 0));
    
    float3 vecHDR = vecColor.rgb;
    float3 vecLDR = ACESFitted(vecHDR);
    vecLDR = pow(vecLDR, 1.f / 2.2f);
    
    gOutputTexture[nDispatchThreadID.xy] = float4(vecLDR, vecColor.a);
}