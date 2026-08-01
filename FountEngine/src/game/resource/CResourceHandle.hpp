#pragma once
#include <cstdint>
#define INVALID_RHANDLE_VALUE 0xFFFFFFFF

class CResourceHandle {
public:
	CResourceHandle() : m_nIndex(INVALID_RHANDLE_VALUE) {}
	CResourceHandle(uint32_t nIndex) : m_nIndex(nIndex) {}

	bool IsValid() const { return m_nIndex != INVALID_RHANDLE_VALUE; }
	uint32_t GetIndex() const { return m_nIndex; }

	bool operator==(const CResourceHandle& Other) const { return Other.m_nIndex == m_nIndex; }

private:
	uint32_t m_nIndex;
};