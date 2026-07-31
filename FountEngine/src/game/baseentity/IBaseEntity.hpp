#pragma once
#include <stdint.h>
#include "entityhandle/CEntityHandle.hpp"

class IBaseEntity {
public:
	virtual ~IBaseEntity() = default;

	virtual void Update(float flDeltaTime) = 0;
	virtual void OnSpawn() = 0;
	virtual void OnDestroy() = 0;
	
	CEntityHandle GetHandle() const { return m_Handle; }

protected:
	friend class CEntitySystem;
	void SetHandle(CEntityHandle Handle) { m_Handle = Handle; }

private:
	CEntityHandle m_Handle;
};