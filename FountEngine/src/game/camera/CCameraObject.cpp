#include "CCameraObject.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/inputsystem/CInputSystem.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "math/mathutils/CMathUtils.hpp"
#include "math/defines.hpp"

CCameraObject::CCameraObject()
	: m_vecPosition(0, 0, 0), m_vecRotation(0, 0, 0),
	m_flCameraSpeed(1.f)
{
	DirectX::XMStoreFloat4x4(&m_mtViewMatrix, DirectX::XMMatrixIdentity());
}

void CCameraObject::Update(float flDeltaTime) {
	static float flSensitivity = 0.002f;

	if (g_pInputSystem->IsMouseButtonDown(0x1) || g_pInputSystem->IsCursorLocked()) {
		m_vecRotation += Vector3_t(
			-flSensitivity * g_pInputSystem->GetMouseDeltaY(),
			flSensitivity * g_pInputSystem->GetMouseDeltaX(),
			0.f
		);

		m_vecRotation.x = CMathUtils::Clamp(m_vecRotation.x, -(M_PIDIV2 - 0.01f), M_PIDIV2 - 0.01f);
	}

	float flPitch = m_vecRotation.x;
	float flYaw = m_vecRotation.y;

	Vector3_t vecForward;
	vecForward.x = cosf(flPitch) * sinf(flYaw);
	vecForward.y = sinf(flPitch);
	vecForward.z = cosf(flPitch) * cosf(flYaw);

	Vector3_t vecRight = Vector3_t(0.f, 1.0f, 0.f).Cross(vecForward).Normalize();
	Vector3_t vecUp = vecForward.Cross(vecRight);

	// Updating keys
	if (g_pInputSystem->IsKeyDown('W')) {
		m_vecPosition += vecForward * m_flCameraSpeed * flDeltaTime;
	}
	if (g_pInputSystem->IsKeyDown('S')) {
		m_vecPosition -= vecForward * m_flCameraSpeed * flDeltaTime;
	}
	if (g_pInputSystem->IsKeyDown('A')) {
		m_vecPosition -= vecRight * m_flCameraSpeed * flDeltaTime;
	}
	if (g_pInputSystem->IsKeyDown('D')) {
		m_vecPosition += vecRight * m_flCameraSpeed * flDeltaTime;
	}
	if (g_pInputSystem->IsKeyDown(VK_SPACE)) {
		m_vecPosition += vecUp * m_flCameraSpeed * flDeltaTime;
	}
	if (g_pInputSystem->IsKeyDown(VK_CONTROL)) {
		m_vecPosition -= vecUp * m_flCameraSpeed * flDeltaTime;
	}

	// Keybinds
	if (g_pInputSystem->IsKeyPressed('L')) {
		if (g_pInputSystem->IsCursorLocked()) g_pInputSystem->UnlockCursor();
		else g_pInputSystem->LockCursor();
	}

	if (g_pInputSystem->IsKeyPressed(VK_ESCAPE)) {
		if (g_pInputSystem->IsCursorLocked()) g_pInputSystem->UnlockCursor();
	}

	if (g_pInputSystem->GetMouseWheelDelta() != 0.f) {
		m_flCameraSpeed = CMathUtils::Clamp(m_flCameraSpeed + g_pInputSystem->GetMouseWheelDelta(), 0.f, 100.f);
		LOG_DEBUG("Set new camera speed - %.1f", m_flCameraSpeed);
	}

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

void CCameraObject::SetSpeed(const float& flCameraSpeed) {
	m_flCameraSpeed = flCameraSpeed;
}

DirectX::XMMATRIX CCameraObject::GetViewMatrix() {
	return DirectX::XMLoadFloat4x4(&m_mtViewMatrix);
}

const Vector3_t& CCameraObject::GetPosition() const {
	return m_vecPosition;
}

const Vector3_t& CCameraObject::GetRotation() const {
	return m_vecRotation;
}

const float& CCameraObject::GetSpeed() const {
	return m_flCameraSpeed;
}
