#pragma once

#include "menu_item_event.h"

class Model;

// Manager of menu items during events processing.
class MenuItemEventsManager : private boost::noncopyable, public boost::signals2::trackable
{
public:
	explicit MenuItemEventsManager(Model& model);

	void Reset();
	void Shutdown();

	std::size_t GetNumEvents() const;
	MenuItemEvent* GetEvent(std::size_t index);
	void EmitEvent(std::size_t index);

private:
	void OnEventRemoved(Event* pEvent);
	void OnEventUpdated(Event* pEvent);

private:
	std::vector<MenuItemEvent*> m_events;
};