#pragma once

class CDXRenderTarget;

class IPostEffect {
public:
	virtual ~IPostEffect() = default;

	virtual void ApplyEffect(CDXRenderTarget* pInput, CDXRenderTarget* pOutput) = 0;
	virtual void OnResize(int nWidth, int nHeight) = 0;
};