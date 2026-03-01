#include "CGameTimer.hpp"
#include <Windows.h>

CGameTimer::CGameTimer()
	: m_flSecondsPerCount(0.0), m_flDeltaTime(-1.0), m_nBaseTime(0), m_nStopTime(0),
	m_nPausedTime(0), m_nPrevTime(0), m_nCurrentTime(0), m_bStopped(false) {
	__int64 nCountsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&nCountsPerSecond);
	m_flSecondsPerCount = 1.f / nCountsPerSecond;
}

float CGameTimer::GetTotalTime() const {
	if (m_bStopped) {
		return static_cast<float>(((m_nStopTime - m_nPausedTime) - m_nBaseTime) * m_flSecondsPerCount);
	}
	else {
		return static_cast<float>(((m_nCurrentTime - m_nPausedTime) - m_nBaseTime) * m_flSecondsPerCount);
	}
}

float CGameTimer::GetDeltaTime() const {
	return static_cast<float>(m_flDeltaTime);
}

void CGameTimer::Reset() {
	__int64 nCurrentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&nCurrentTime);

	m_nBaseTime = nCurrentTime;
	m_nPrevTime = nCurrentTime;
	m_nStopTime = 0;
	m_bStopped = false;
}

void CGameTimer::Start() {
	if (!m_bStopped) return;

	__int64 nStartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&nStartTime);

	m_nPausedTime += (nStartTime - m_nStopTime);
	m_nPrevTime = nStartTime;
	m_nStopTime = 0;
	m_bStopped = false;
}

void CGameTimer::Stop() {
	if (m_bStopped) return;

	__int64 nCurrentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&nCurrentTime);

	m_nStopTime = nCurrentTime;
	m_bStopped = true;
}

void CGameTimer::Tick() {
	if (m_bStopped) {
		m_flDeltaTime = 0.0f;
		return;
	}

	__int64 nCurrentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&nCurrentTime);

	m_nCurrentTime = nCurrentTime;
	m_flDeltaTime = (m_nCurrentTime - m_nPrevTime) * m_flSecondsPerCount;
	m_nPrevTime = m_nCurrentTime;

	if (m_flDeltaTime < 0.0f) {
		m_flDeltaTime = 0.0f;
	}
}
