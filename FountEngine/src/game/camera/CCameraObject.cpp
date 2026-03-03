#include "CCameraObject.hpp"

CCameraObject::CCameraObject() : m_vecPosition(0, 0, 0), m_vecRotation(0, 0, 0) {
	DirectX::XMStoreFloat4x4(&m_mtViewMatrix, DirectX::XMMatrixIdentity());
}

void CCameraObject::Update(float flDeltaTime) {
	static float flSensitivity = 1.0f;

	float flPitch = m_vecRotation.x;
	float flYaw = m_vecRotation.y;

	Vector3_t vecForward;
	vecForward.x = cosf(flPitch) * sinf(flYaw);
	vecForward.y = sinf(flPitch);
	vecForward.z = cosf(flPitch) * cosf(flYaw);

	Vector3_t vecRight = Vector3_t(0.f, 1.0f, 0.f).Cross(vecForward).Normalize();
	Vector3_t vecUp = vecForward.Cross(vecRight);

	DirectX::XMVECTOR xmPos = m_vecPosition.DXAsVector();
	DirectX::XMVECTOR xmTarget = DirectX::XMVectorAdd(xmPos, vecForward.DXAsVector());
	DirectX::XMVECTOR xmUp = vecUp.DXAsVector();

	DirectX::XMMATRIX mtView = DirectX::XMMatrixLookAtLH(xmPos, xmTarget, xmUp);
	DirectX::XMStoreFloat4x4(&m_mtViewMatrix, mtView);
}

void CCameraObject::SetPosition(const Vector3_t& vecPosition) {
	m_vecPosition = vecPosition;
}

void CCameraObject::SetRotation(const Vector3_t& vecRotation) {
	m_vecRotation = vecRotation;
}

const DirectX::XMFLOAT4X4& CCameraObject::GetViewMatrix() const {
	return m_mtViewMatrix;
}

const Vector3_t& CCameraObject::GetPosition() const {
	return m_vecPosition;
}

const Vector3_t& CCameraObject::GetRotation() const {
	return m_vecRotation;
}
