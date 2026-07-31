#pragma once
#include <cstdint>
#define ENT_ENTRY_MASK 0x7FFF
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define NUM_SERIAL_NUM_SHIFT_BITS 15

class CEntityHandle {
public:
	CEntityHandle() : m_nIndex(INVALID_EHANDLE_INDEX) { }
	CEntityHandle(uint32_t nIndex, uint32_t nSerial)
		: m_nIndex(nIndex | (nSerial << NUM_SERIAL_NUM_SHIFT_BITS)) { }

	uint32_t GetIndex() const { return m_nIndex & ENT_ENTRY_MASK; }
	uint32_t GetSerial() const { return m_nIndex >> NUM_SERIAL_NUM_SHIFT_BITS; }
	bool IsValid() const { return m_nIndex != INVALID_EHANDLE_INDEX; }

private:
	uint32_t m_nIndex;
};