#include "CSpotLight.hpp"
#include "enums/entity_flags.hpp"

uint32_t CSpotLight::GetFlags() const {
    return CBaseLightEntity::GetFlags() | ENT_FLAG_LIGHT_SPOT;
}
