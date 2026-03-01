#pragma once

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

	__int64 m_nBaseTime;
	__int64 m_nPausedTime;
	__int64 m_nStopTime;
	__int64 m_nPrevTime;
	__int64 m_nCurrentTime;

	bool m_bStopped;
};