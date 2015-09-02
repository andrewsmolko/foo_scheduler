#pragma once

#include "root_controller.h"
#include "model.h"
#include "menu_item_events_manager.h"
#include "player_events_manager.h"
#include "date_time_events_manager.h"
#include "timers_manager.h"
#include "prototypes_manager.h"

class ServiceManager : boost::noncopyable
{
public:
	static ServiceManager& Instance();

	RootController& GetRootController();
	Model& GetModel();
	MenuItemEventsManager& GetMenuItemEventsManager();
	PlayerEventsManager& GetPlayerEventsManager();
	DateTimeEventsManager& GetDateTimeEventsManager();
	TimersManager& GetTimersManager();
	PrototypesManager<Event>& GetEventPrototypesManager();
	PrototypesManager<IAction>& GetActionPrototypesManager();

private:
	ServiceManager();

private:
	RootController m_rootController;
	Model m_model;
	MenuItemEventsManager m_menuItemEventsManager;
	PlayerEventsManager m_playerEventsManager;
	DateTimeEventsManager m_dateTimeEventsManager;
	TimersManager m_timersManager;
	PrototypesManager<Event> m_eventPrototypesManager;
	PrototypesManager<IAction> m_actionPrototypesManager;
};


