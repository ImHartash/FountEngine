#include "postprocess_common.hlsl"

static const int gBlockSize = 4;

[numthreads(8, 8, 1)]
void main(uint3 nDispatchThreadID : SV_DispatchThreadID) {
    uint nWidth, nHeight;
    gOutputTexture.GetDimensions(nWidth, nHeight);
    if (nDispatchThreadID.x >= nWidth || nDispatchThreadID.y >= nHeight) 
        return;
    
    uint bx = (nDispatchThreadID.x / gBlockSize) * gBlockSize;
    uint by = (nDispatchThreadID.y / gBlockSize) * gBlockSize;

    uint cx = min(bx + gBlockSize / 2, nWidth - 1);
    uint cy = min(by + gBlockSize / 2, nHeight - 1);
    
    gOutputTexture[nDispatchThreadID.xy] = gInputTexture.Load(int3(cx, cy, 0));
}