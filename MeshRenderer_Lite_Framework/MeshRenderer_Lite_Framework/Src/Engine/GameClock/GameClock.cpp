#include <Utilities/precompiled.h>
#include <Engine/GameClock/GameClock.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

GameClock::GameClock() : m_SecondsPerCount(0.0),
m_Dt(-1.0), m_BaseTime(0),
m_PausedTime(0),
m_StoppedTime(0),
m_PreviousTime(0),
m_CurrentTime(0),
m_IsStopped(false)
{
	//Calculate how many seconds pass between each processor tick
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}

GameClock::~GameClock()
{
}

float GameClock::GetTotalTime() const
{
	//Calculates the elapsed time since reset was last called. Not including paused
	//time.

	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

	if (m_IsStopped)
	{
		return (float)(((m_StoppedTime - m_PausedTime) - m_BaseTime)*m_SecondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime

	else
	{
		return (float)(((m_CurrentTime - m_PausedTime) - m_BaseTime)*m_SecondsPerCount);
	}
}

float GameClock::GetDeltaTime() const
{
	return static_cast<float>(m_Dt);
}

void GameClock::ResetClock()
{
	//Clock is reset
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	//Reset the clock counts, this will be the baseline time
	m_BaseTime = m_PreviousTime = currentTime;
	m_StoppedTime = 0;
	m_IsStopped = false;
}

void GameClock::StartClock()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     
	if (m_IsStopped)
	{
		//delta time since last frame
		m_PausedTime += (startTime - m_StoppedTime);

		m_PreviousTime = startTime;
		m_StoppedTime = 0;
		m_IsStopped = false;
	}
}

void GameClock::StopClock()
{
	//If the clock is running, stop it and start counting
	if (!m_IsStopped)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		m_StoppedTime = currentTime;
		m_IsStopped = true;
	}
}

void GameClock::Tick()
{
	//If the clock is stopped, delta time is 0
	if (m_IsStopped)
	{
		m_Dt = 0.0;
	}

	else
	{
		//Calculate current time
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		m_CurrentTime = currentTime;

		//Time elapsed between last frame and current frame in seconds
		m_Dt = (m_CurrentTime - m_PreviousTime) * m_SecondsPerCount;

		//Save current time for next frame
		m_PreviousTime = m_CurrentTime;

		//Clamp delta time to be a positive value
		if (m_Dt < 0.0)
			m_Dt = 0.0;
	}
}