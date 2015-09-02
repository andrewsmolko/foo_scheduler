#include "pch.h"
#include "date_time_events_manager.h"
#include "service_manager.h"

DateTimeEventsManager::DateTimeEventsManager(Model& model)
{
	model.ConnectModelStateChangedSlot(
		boost::bind(&DateTimeEventsManager::Reset, this));
}

void DateTimeEventsManager::Reset()
{
	StopTimer();
	UpdatePendingEvents();
}

void DateTimeEventsManager::Shutdown()
{
	StopTimer();
}

void DateTimeEventsManager::OnTimerEvent(TimersManager::TimerID timerID)
{
	_ASSERTE(m_currentPendingEvent != boost::none);
	_ASSERTE(m_currentPendingEvent->second == timerID);

	ServiceManager::Instance().GetTimersManager().CloseTimer(m_currentPendingEvent->second);
	ServiceManager::Instance().GetRootController().ProcessEvent(m_currentPendingEvent->first);

	m_currentPendingEvent = boost::none;

	UpdatePendingEvents();
}

std::vector<DateTimeEvent*> DateTimeEventsManager::GetDateTimeEvents() const
{
	std::vector<DateTimeEvent*> dateTimeEvents;

	const std::vector<Event*> events = ServiceManager::Instance().GetModel().GetEvents();
	for (auto it = events.begin(); it != events.end(); ++it)
	{
		Event* event = *it;

		if (DateTimeEvent* pEvent = dynamic_cast<DateTimeEvent*>(event))
		{
			if (pEvent->IsEnabled())
				dateTimeEvents.push_back(pEvent);
		}
	}

	return dateTimeEvents;
}

void DateTimeEventsManager::StopTimer()
{
	// Destroy callback controller, so even if there are pending callbacks in the message loop,
	// they will not be executed.
	m_onTimerEventProxy.reset();

	if (m_currentPendingEvent != boost::none)
	{
		ServiceManager::Instance().GetTimersManager().CloseTimer(m_currentPendingEvent->second);
		m_currentPendingEvent = boost::none;
	}
}

boost::posix_time::ptime DateTimeEventsManager::GetEventStartTime(const DateTimeEvent* pEvent) const
{
	boost::posix_time::ptime resultTime;

	switch (pEvent->GetType())
	{
	case DateTimeEvent::typeOnce:
		{
			boost::posix_time::ptime scheduledTime(pEvent->GetDate(), pEvent->GetTime());
			const boost::posix_time::time_duration d = boost::posix_time::second_clock::local_time() - scheduledTime;

			// This may happen after player startup if there is an event with date/time in the past.
			if (d.total_seconds() > 1)
			{
				scheduledTime = boost::posix_time::not_a_date_time;
			}

			resultTime = scheduledTime;
		}
		break;

	case DateTimeEvent::typeDaily:
		{
			boost::posix_time::ptime startTime(boost::gregorian::day_clock::local_day(), pEvent->GetTime());
			
			if (boost::posix_time::second_clock::local_time() >= startTime)
				startTime += boost::gregorian::days(1);

			resultTime = startTime;
		}
		break;

	case DateTimeEvent::typeWeekly:
		{
			boost::gregorian::date dateNow(boost::gregorian::day_clock::local_day());

			int currentDayOfWeek = dateNow.day_of_week().as_number();

			if (currentDayOfWeek == 0)
				currentDayOfWeek = 7;

			--currentDayOfWeek; // Format 0 = Mon, 1 = Tue, .. , 6 = Sun
			
			boost::gregorian::date weekStartDate =
				dateNow - boost::gregorian::date_duration(currentDayOfWeek);

			std::vector<boost::posix_time::ptime> times;

			for (int i = 0; i < 7; ++i)
				if (pEvent->GetWeekDays() & (1 << i))
				{
					boost::posix_time::ptime startTime(
						weekStartDate + boost::gregorian::date_duration(i), pEvent->GetTime());

					if (boost::posix_time::second_clock::local_time() >= startTime)
						startTime += boost::gregorian::days(7);

					times.push_back(startTime);
				}

			std::sort(times.begin(), times.end());

			_ASSERTE(!times.empty());
			resultTime = times[0];
		}
		break;
	}

	return resultTime;
}

std::vector<DateTimeEventsManager::EventStartTimePair> DateTimeEventsManager::GetEventsNearestStartTime() const
{
	std::vector<EventStartTimePair> result;

	const std::vector<DateTimeEvent*> dateTimeEvents = GetDateTimeEvents();
	for (auto it = dateTimeEvents.begin(); it != dateTimeEvents.end(); ++it)
	{
		DateTimeEvent* event = *it;

		const boost::posix_time::ptime eventStartTime = GetEventStartTime(event);

		// Skip once events in the past.
		if (eventStartTime.is_not_a_date_time())
		{
			continue;
		}

		result.push_back(std::make_pair(event, eventStartTime));
	}

	std::sort(result.begin(), result.end(),
		boost::bind(&EventStartTimePair::second, _1) < boost::bind(&EventStartTimePair::second, _2));

	return result;
}

void DateTimeEventsManager::UpdatePendingEvents()
{
	const std::vector<EventStartTimePair> eventsNearestStartTime = GetEventsNearestStartTime();

	if (ServiceManager::Instance().GetModel().IsSchedulerEnabled() && !eventsNearestStartTime.empty())
	{
		m_onTimerEventProxy.reset(
			new MethodCallProxy(boost::bind(&DateTimeEventsManager::OnTimerEvent, this, _1)));

		const EventStartTimePair& nearestEventTimePair = eventsNearestStartTime[0];
		const TimersManager::TimerID timerID = ServiceManager::Instance().GetTimersManager().CreateTimer(
			nearestEventTimePair.second, boost::posix_time::not_a_date_time, nearestEventTimePair.first->GetWakeup());

		m_currentPendingEvent = std::make_pair(nearestEventTimePair.first, timerID);

		// Proxy forwards OnTimerEvent invocation to DateTimeEventsManager::OnTimerEvent.
		AsyncCall::CallbackPtr pTimerCallback =
			AsyncCall::MakeCallback<MethodCallProxy>(m_onTimerEventProxy,
				boost::bind(&MethodCallProxy::OnTimerEvent, m_onTimerEventProxy.get(), timerID));

		ServiceManager::Instance().GetTimersManager().StartTimer(timerID, pTimerCallback);
	}

	m_pendingEvents.clear();

	for (auto it = eventsNearestStartTime.begin(); it != eventsNearestStartTime.end(); ++it)
	{
		const auto& eventStartTime = *it;
		m_pendingEvents.push_back(eventStartTime.first);
	}

	m_pendingEventsUpdatedSignal();
}

boost::signals2::connection DateTimeEventsManager::ConnectPendingEventsUpdatedSlot(const PendingEventsUpdated::slot_type& slot)
{
	return m_pendingEventsUpdatedSignal.connect(slot);
}

std::vector<DateTimeEvent*> DateTimeEventsManager::GetPendingEvents() const
{
	return m_pendingEvents;
}