#pragma once
#include "math/defines.hpp"
#include "math/types/vector3.hpp"
#include <DirectXMath.h>

struct Angle_t {
	float flPitch;
	float flYaw;
	float flRoll;

	Angle_t() : flPitch(0.f), flYaw(0.f), flRoll(0.f) {}
	Angle_t(float pitch, float yaw, float roll) 
		: flPitch(pitch), flYaw(yaw), flRoll(roll) {}

	static Angle_t FromDegrees(float pitch, float yaw, float roll) {
		return Angle_t(
			DEG_TO_RAD(pitch),
			DEG_TO_RAD(yaw),
			DEG_TO_RAD(roll)
		);
	}

	Angle_t& operator+=(const Angle_t& angOther) {
		flPitch += angOther.flPitch;
		flYaw += angOther.flYaw;
		flRoll += angOther.flRoll;
		return *this;
	}

	Angle_t& operator-=(const Angle_t& angOther) {
		flPitch -= angOther.flPitch;
		flYaw -= angOther.flYaw;
		flRoll -= angOther.flRoll;
		return *this;
	}

	Angle_t operator*(const float flScalar) const {
		return Angle_t(flPitch * flScalar, flYaw * flScalar, flRoll * flScalar);
	}

	Vector3_t ToDegrees() const {
		return Vector3_t(
			RAD_TO_DEG(flPitch),
			RAD_TO_DEG(flYaw),
			RAD_TO_DEG(flRoll)
		);
	}
};