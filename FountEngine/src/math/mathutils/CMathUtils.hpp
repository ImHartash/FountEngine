#pragma once

class CMathUtils {
public:
	template<typename T>
	static T Min(const T& a, const T& b) {
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t) {
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high) {
		return x < low ? low : (x > high ? high : x);
	}
};