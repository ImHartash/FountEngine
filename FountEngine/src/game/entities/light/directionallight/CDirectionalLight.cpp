#include "CDirectionalLight.hpp"
#include "enums/entity_flags.hpp"

uint32_t CDirectionalLight::GetFlags() const {
    return CBaseLightEntity::GetFlags() | ENT_FLAG_LIGHT_DIR;
}
