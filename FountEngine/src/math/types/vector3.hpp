#pragma once
#define 
#include <DirectXMath.h>

struct Vector3_t {
	float x = 0.f, y = 0.f, z = 0.f;

	Vector3_t() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3_t(float flX, float flY, float flZ) : x(flX), y(flY), z(flZ) {}

	Vector3_t& operator+=(const Vector3_t& vecOther) {
		x += vecOther.x;
		y += vecOther.y;
		z += vecOther.z;
		return *this;
	}

	Vector3_t& operator-=(const Vector3_t& vecOther) {
		x -= vecOther.x;
		y -= vecOther.y;
		z -= vecOther.z;
		return *this;
	}

	Vector3_t& operator*=(const float flScalar) {
		x *= flScalar;
		y *= flScalar;
		z *= flScalar;
		return *this;
	}

	Vector3_t& operator/=(const float flScalar) {
		x /= flScalar;
		y /= flScalar;
		z /= flScalar;
		return *this;
	}

	Vector3_t operator+(const Vector3_t& vecOther) const {
		return Vector3_t(x + vecOther.x, y + vecOther.y, z + vecOther.z);
	}

	Vector3_t operator-(const Vector3_t& vecOther) const {
		return Vector3_t(x - vecOther.x, y - vecOther.y, z - vecOther.z);
	}

	Vector3_t operator*(float flScalar) const {
		return Vector3_t(x * flScalar, y * flScalar, z * flScalar);
	}

	Vector3_t operator/(float flScalar) const {
		return Vector3_t(x / flScalar, y / flScalar, z / flScalar);
	}

	bool operator==(const Vector3_t& vecOther) const {
		return x == vecOther.x && y == vecOther.y && z == vecOther.z;
	}

	bool operator!=(const Vector3_t& vecOther) const {
		return !(*this == vecOther);
	}
};