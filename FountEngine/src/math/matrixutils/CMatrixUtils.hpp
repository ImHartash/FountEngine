#pragma once
#include <DirectXMath.h>

class CMatrixUtils {
public:
	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX Matrix) {
		DirectX::XMMATRIX mtCopy = Matrix;
		mtCopy.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR vecDeterminant 
			= DirectX::XMMatrixDeterminant(mtCopy);
		return DirectX::XMMatrixTranspose(
			DirectX::XMMatrixInverse(&vecDeterminant, mtCopy));
	}
};