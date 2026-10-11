#include "CBaseLightEntity.hpp"
#include "enums/entity_flags.hpp"

uint32_t CBaseLightEntity::GetFlags() const {
	return ENT_FLAG_TYPE_LIGHT_ENTITY;
}