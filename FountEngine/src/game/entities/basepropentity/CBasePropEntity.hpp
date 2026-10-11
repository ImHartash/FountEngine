#pragma once
#include "../basemodelentity/CBaseModelEntity.hpp"

class CBasePropEntity : public CBaseModelEntity {
public:
	explicit CBasePropEntity(const std::string& strModelPath) { this->SetModelResource(strModelPath); }
};