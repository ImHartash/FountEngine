#pragma once
#include <stdint.h>

class IBaseEntity {
public:
	virtual ~IBaseEntity() = default;

	virtual void Update(float flDeltaTime) = 0;
	virtual void OnSpawn() = 0;
	virtual void OnDestroy() = 0;
	
	uint32_t GetIndex() const { return m_nIndex; };
	void SetIndex(uint32_t nIndex) { m_nIndex = nIndex; }

private:
	uint32_t m_nIndex;
};