#pragma once

#include "date_time_event.h"
#include "timers_manager.h"
#include "async_call.h"

class MethodCallProxy
{
public:
	explicit MethodCallProxy(const boost::function<void (TimersManager::TimerID)>& func) :
		m_func(func) {}

	void OnTimerEvent(TimersManager::TimerID timerID)
	{
		m_func(timerID);
	}

private:
	boost::function<void (TimersManager::TimerID)> m_func;
};

class Model;

class DateTimeEventsManager : private boost::noncopyable, public boost::signals2::trackable
{
public:
	explicit DateTimeEventsManager(Model& model);

	void Reset();
	void Shutdown();

	typedef boost::signals2::signal<void ()> PendingEventsUpdated;
	boost::signals2::connection ConnectPendingEventsUpdatedSlot(const PendingEventsUpdated::slot_type& slot);

	std::vector<DateTimeEvent*> GetPendingEvents() const;

private:
	void StopTimer();
	void UpdatePendingEvents();

	std::vector<DateTimeEvent*> GetDateTimeEvents() const;
	void OnTimerEvent(TimersManager::TimerID timerID);

	boost::posix_time::ptime GetEventStartTime(const DateTimeEvent* pEvent) const;

	typedef std::pair<DateTimeEvent*, boost::posix_time::ptime> EventStartTimePair;
	std::vector<EventStartTimePair> GetEventsNearestStartTime() const;

private:
	boost::shared_ptr<MethodCallProxy> m_onTimerEventProxy;
	boost::optional<std::pair<DateTimeEvent*, TimersManager::TimerID>> m_currentPendingEvent;

	// Pending events. The first event is the nearest.
	std::vector<DateTimeEvent*> m_pendingEvents;

	PendingEventsUpdated m_pendingEventsUpdatedSignal;
};