#include "CPointLight.hpp"
#include "enums/entity_flags.hpp"

uint32_t CPointLight::GetFlags() const {
    return CBaseLightEntity::GetFlags() | ENT_FLAG_LIGHT_POINT;
}
