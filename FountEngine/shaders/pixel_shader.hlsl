#include "common.hlsl"

float4 FntPixelShader(VertexOut_t PixelIn) : SV_TARGET
{
    PixelIn.vNormalWorld = normalize(PixelIn.vNormalWorld);
    
    float3 vToEye = normalize(gCameraPosition - PixelIn.vPositionWorld);
    
    float3 vAmbient = float3(0, 0, 0);
    float3 vDiffuse = float3(0, 0, 0);
    float3 vSpecular = float3(0, 0, 0);
    
    float3 A, D, S;
    
    [unroll]
    for (uint i = 0; i < gDirectionalLightsNumber; ++i)
    {
        ComputeDirectionalLight(gMaterial, gDirectionalLights[i], PixelIn.vNormalWorld, vToEye, A, D, S);
        vAmbient += A;
        vDiffuse += D;
        vSpecular += S;
    }
    
    [loop]
    for (uint j = 0; j < gPointLightsNumber; ++j)
    {
        ComputePointLight(gMaterial, gPointLights[j], PixelIn.vPositionWorld, PixelIn.vNormalWorld, vToEye, A, D, S);
        vAmbient += A;
        vDiffuse += D;
        vSpecular += S;
    }
    
    [loop]
    for (uint k = 0; k < gSpotLightsNumber; ++k)
    {
        ComputeSpotLight(gMaterial, gSpotLights[k], PixelIn.vPositionWorld, PixelIn.vNormalWorld, vToEye, A, D, S);
        vAmbient += A;
        vDiffuse += D;
        vSpecular += S;
    }

    float4 vTextureColor = gTexture.Sample(gSampler, PixelIn.vTexcoord);
    float3 vLightColor = vTextureColor.rgb * (vAmbient + vDiffuse) + vSpecular;
    return float4(vLightColor, gMaterial.flOpacity * vTextureColor.a);
}