#pragma once

class GameClock
{
public:
	//CONSTRUCTORS
	GameClock(void);
	~GameClock(void);

	//GETTERS
	float GetTotalTime(void) const;
	float GetDeltaTime(void) const;

	//UTILITY
	void ResetClock(void);
	void StartClock(void);
	void StopClock(void);
	void Tick(void);

private:
	double m_SecondsPerCount;
	double m_Dt;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StoppedTime;
	__int64 m_PreviousTime;
	__int64 m_CurrentTime;

	bool m_IsStopped;

};