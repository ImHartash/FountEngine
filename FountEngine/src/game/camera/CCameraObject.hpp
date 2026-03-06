#pragma once
#include <DirectXMath.h>
#include "math/types/vector3.hpp"

class CCameraObject {
public:
	CCameraObject();
	~CCameraObject() = default;

	void Update(float flDeltaTime);

	void SetPosition(const Vector3_t& vecPosition);
	void SetRotation(const Vector3_t& vecRotation);

	DirectX::XMMATRIX GetViewMatrix();
	const Vector3_t& GetPosition() const;
	const Vector3_t& GetRotation() const;

private:
	Vector3_t m_vecPosition;
	Vector3_t m_vecRotation;

	DirectX::XMFLOAT4X4 m_mtViewMatrix;
};