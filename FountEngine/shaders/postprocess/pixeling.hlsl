#include "postprocess_common.hlsl"

static const int gBlockSize = 4;
static const int gColorLevels = 24;

static const float Bayer4x4[16] =
{
    0, 8, 2, 10,
    12, 4, 14, 6,
     3, 11, 1, 9,
    15, 7, 13, 5
};

[numthreads(8, 8, 1)]
void main(uint3 nDispatchThreadID : SV_DispatchThreadID) {
    uint nWidth, nHeight;
    gOutputTexture.GetDimensions(nWidth, nHeight);
    if (nDispatchThreadID.x >= nWidth || nDispatchThreadID.y >= nHeight) 
        return;
    
    uint bbx = nDispatchThreadID.x / gBlockSize;
    uint bby = nDispatchThreadID.y / gBlockSize;

    uint cx = min(bbx * gBlockSize + gBlockSize / 2, nWidth - 1);
    uint cy = min(bby * gBlockSize + gBlockSize / 2, nHeight - 1);
    
    // Dither
    uint nBlockID = (bby & 3) * 4 + (bbx & 3);
    float flBayer = Bayer4x4[nBlockID] * (1.f / 16.f);
    
    float flLevels = (float) (gColorLevels - 1);
    float4 vecColor = gInputTexture.Load(int3(cx, cy, 0));
    vecColor.rgb = saturate(vecColor.rgb);
    vecColor.rgb = floor(vecColor.rgb * flLevels + flBayer) / flLevels;
    
    gOutputTexture[nDispatchThreadID.xy] = vecColor;
}