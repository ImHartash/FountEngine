#pragma once
#include <cstdint>

class CGameTimer {
public:
	CGameTimer();
	~CGameTimer() = default;

	float GetTotalTime() const;
	float GetDeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double m_flSecondsPerCount;
	double m_flDeltaTime;

	int64_t m_nBaseTime;
	int64_t m_nPausedTime;
	int64_t m_nStopTime;
	int64_t m_nPrevTime;
	int64_t m_nCurrentTime;

	bool m_bStopped;
};