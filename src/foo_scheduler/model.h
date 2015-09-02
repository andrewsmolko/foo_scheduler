#pragma once

#include "event.h"
#include "action.h"
#include "action_list.h"
#include "foobar_stream.h"

struct ModelState
{
	typedef boost::ptr_vector<Event> EventsContainer;
	typedef boost::ptr_vector<ActionList> ActionListsContainer;

	ModelState() : schedulerEnabled(true) {}

	void Reset()
	{
		schedulerEnabled = true;
		events.clear();
		actionLists.clear();
	}

	bool schedulerEnabled;
	EventsContainer events;
	ActionListsContainer actionLists;
};

class Model : boost::noncopyable
{
public:
	Model();

	void Load(foobar_stream_reader& stream);
	void Save(foobar_stream_writer& stream) const;

	std::vector<Event*> GetEvents();
	std::vector<ActionList*> GetActionLists();

	ActionList* GetActionListByGUID(const GUID& guid);

	void UpdateEvent(Event* pEvent);
	void RemoveEvent(Event* pEvent);

	const std::vector<int>& GetEventsWindowColumnsWidths() const;
	void SetEventsWindowColumnsWidths(const std::vector<int>& cw);

	const ModelState& GetState() const;

	// Signals ModelStateChanged.
	void SetState(const ModelState& state);

	bool IsSchedulerEnabled() const;
	
public:
	typedef boost::signals2::signal<void (Event*)> EventUpdatedSignal;
	typedef boost::signals2::signal<void (Event*)> EventRemovedSignal;
	typedef boost::signals2::signal<void ()> ModelStateChanged;

	boost::signals2::connection ConnectEventUpdatedSlot(const EventUpdatedSignal::slot_type& slot);
	boost::signals2::connection ConnectEventRemovedSlot(const EventRemovedSignal::slot_type& slot);
	boost::signals2::connection ConnectModelStateChangedSlot(const ModelStateChanged::slot_type& slot);

private:
	ModelState m_modelState;

	std::vector<int> m_eventsWindowColumnsWidths;

	EventUpdatedSignal m_eventUpdatedSignal;
	EventRemovedSignal m_eventRemovedSignal;
	ModelStateChanged m_modelStateChanged;
};