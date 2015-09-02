#include "pch.h"
#include "timers_manager.h"
#include "async_call.h"

TimersManager::TimersManager() : m_nextTimerID(0)
{
}

void TimersManager::Init()
{
}

void TimersManager::Shutdown()
{
	_ASSERTE(m_timerID2TimerHandle.empty());
	_ASSERTE(m_timerID2Descr.empty());
}

TimersManager::TimerID TimersManager::CreateTimer(
	const boost::posix_time::ptime& startTime, const boost::posix_time::time_duration& period, bool wakeup)
{
	LARGE_INTEGER startTimeUTC = PTime2LARGE_INTEGER(startTime);

	HANDLE hTimerEvent = CreateWaitableTimer(NULL, FALSE, NULL);
	
	SetWaitableTimer(hTimerEvent, &startTimeUTC,
		period.is_not_a_date_time() ? 0 : static_cast<LONG>(period.total_milliseconds()),
		NULL, NULL, wakeup);

	TimerID timerID = m_nextTimerID++;
	m_timerID2TimerHandle.insert(std::make_pair(timerID, hTimerEvent));

	return timerID;
}

void TimersManager::StartTimer(TimerID timerID, const AsyncCall::CallbackPtr& pCallback)
{
	auto it = m_timerID2TimerHandle.find(timerID);
	_ASSERTE(it != m_timerID2TimerHandle.end());

	HANDLE waitObjectHandle = INVALID_HANDLE_VALUE;

	RegisterWaitForSingleObject(&waitObjectHandle, it->second, DoneWaiting,
		pCallback.get(), INFINITE, WT_EXECUTEINWAITTHREAD);

	m_timerID2Descr.insert(std::make_pair(timerID, TimerDescr(waitObjectHandle, pCallback)));
}

void TimersManager::CloseTimer(TimerID timerID)
{
	HANDLE timerHandle = CancelTimerWithID(timerID);

	CancelWaitableTimer(timerHandle);
	
	auto it = m_timerID2Descr.find(timerID);
	_ASSERTE(it != m_timerID2Descr.end());

	// UnregisterWaitEx with the parameter equal to INVALID_HANDLE_VALUE
	// waits for all callback functions to complete before returning.
	// So callback pointer will be always deleted, even if TimersManager::DoneWaiting is running.
	UnregisterWaitEx(it->second.waitObjectHandle, INVALID_HANDLE_VALUE);

	m_timerID2Descr.erase(it);

	CloseHandle(timerHandle);
}

LARGE_INTEGER TimersManager::PTime2LARGE_INTEGER(const boost::posix_time::ptime& pt)
{
	SYSTEMTIME st = {0};

	st.wYear  = pt.date().year();
	st.wMonth = pt.date().month();
	st.wDay   = pt.date().day();

	st.wHour   = static_cast<WORD>(pt.time_of_day().hours());
	st.wMinute = static_cast<WORD>(pt.time_of_day().minutes());
	st.wSecond = static_cast<WORD>(pt.time_of_day().seconds());

	FILETIME ftLocal, ftUTC;
	SystemTimeToFileTime(&st, &ftLocal);
	LocalFileTimeToFileTime(&ftLocal, &ftUTC);

	LARGE_INTEGER liUTC;

	// Convert FILETIME to LARGE_INTEGER because of different alignment.
	liUTC.LowPart  = ftUTC.dwLowDateTime;
	liUTC.HighPart = ftUTC.dwHighDateTime;

	return liUTC;
}

void CALLBACK TimersManager::DoneWaiting(void* param, BOOLEAN timedOut)
{
	// At this point it's guaranteed that the callback pointer has valid reference count,
	// cause when a timer is closed, UnregisterWaitEx waits for callback to complete. Only AFTER that
	// reference count is decreased in CloseTimer. So the situation when CloseTimer has already destroyed
	// the callback pointer but this callback is still running is impossible.
	AsyncCall::CallbackPtr pCallback(static_cast<AsyncCall::ICallback*>(param));
	AsyncCall::AsyncRunInMainThread(pCallback);
}

HANDLE TimersManager::CancelTimerWithID(TimerID timerID)
{
	auto it = m_timerID2TimerHandle.find(timerID);
	_ASSERTE(it != m_timerID2TimerHandle.end());

	HANDLE timerHandle = it->second;

	m_timerID2TimerHandle.erase(it);

	return timerHandle;
}

