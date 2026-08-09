#include "CSpaceObject.hpp"
#include "math/defines.hpp"
#include <DirectXMath.h>

void CSpaceObject::Update(float flDeltaTime) {
	m_flOrbitAngle += m_flOrbitSpeed * flDeltaTime;
	if (m_flOrbitAngle > M_2PI)
		m_flOrbitAngle -= M_2PI;

	m_angViewAngle.flYaw += m_flSpinSpeed * flDeltaTime;
	if (m_angViewAngle.flYaw > M_2PI)
		m_angViewAngle.flYaw -= M_2PI;

	Vector3_t vecCenterPosition = m_pCenterEntity ? 
		m_pCenterEntity->GetPosition() : Vector3_t(0, 0, 0);

	Vector3_t vecNewPosition;
	vecNewPosition.x = vecCenterPosition.x + m_flOrbitRadius * cosf(m_flOrbitAngle);
	vecNewPosition.y = vecCenterPosition.y;
	vecNewPosition.z = vecCenterPosition.z + m_flOrbitRadius * sinf(m_flOrbitAngle);

	m_vecPosition = vecNewPosition;
}
