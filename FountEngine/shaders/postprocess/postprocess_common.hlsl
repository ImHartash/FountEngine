Texture2D<float4> gInputTexture : register(t0);
RWTexture2D<float4> gOutputTexture : register(u0);

cbuffer cbScreenParameters : register(b0)
{
    uint nWidth;
    uint nHeight;
}