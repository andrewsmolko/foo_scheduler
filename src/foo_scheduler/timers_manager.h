#pragma once

#include "async_call.h"

//------------------------------------------------------------------------------
// TimersManager
//------------------------------------------------------------------------------

class TimersManager : private boost::noncopyable
{
public:
	TimersManager();

	void Init();
	void Shutdown();

	typedef __int64 TimerID;
	static const TimerID invalidTimerID = -1;

	// If period is not_a_date_time, then it's a one shot timer.
	TimerID CreateTimer(const boost::posix_time::ptime& startTime,
		const boost::posix_time::time_duration& period, bool wakeup);

	void StartTimer(TimerID timerID, const AsyncCall::CallbackPtr& pCallback);

	void CloseTimer(TimerID timerID);

private:
	LARGE_INTEGER PTime2LARGE_INTEGER(const boost::posix_time::ptime& pt);
	HANDLE CancelTimerWithID(TimerID timerID);

	// Called on a background thread when done waiting.
	static void CALLBACK DoneWaiting(void* param, BOOLEAN timedOut);

private:
	TimerID m_nextTimerID;

	std::map<TimerID, HANDLE> m_timerID2TimerHandle;

	struct TimerDescr
	{
		HANDLE waitObjectHandle;
		AsyncCall::CallbackPtr pCallback;

		TimerDescr() : waitObjectHandle(INVALID_HANDLE_VALUE) {}
		TimerDescr(HANDLE h, const AsyncCall::CallbackPtr& c) : waitObjectHandle(h), pCallback(c) {}
	};

	std::map<TimerID, TimerDescr> m_timerID2Descr;
};