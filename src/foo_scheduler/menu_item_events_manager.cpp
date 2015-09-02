#include "pch.h"
#include "menu_item_events_manager.h"
#include "service_manager.h"

MenuItemEventsManager::MenuItemEventsManager(Model& model)
{
	model.ConnectEventRemovedSlot(
		boost::bind(&MenuItemEventsManager::OnEventRemoved, this, _1));

	model.ConnectEventUpdatedSlot(
		boost::bind(&MenuItemEventsManager::OnEventUpdated, this, _1));

	model.ConnectModelStateChangedSlot(
		boost::bind(&MenuItemEventsManager::Reset, this));
}

void MenuItemEventsManager::Reset()
{
	m_events.clear();

	if (!ServiceManager::Instance().GetModel().IsSchedulerEnabled())
		return;

	std::vector<Event*> events = ServiceManager::Instance().GetModel().GetEvents();

	for (std::size_t i = 0; i < events.size(); ++i)
	{
		if (MenuItemEvent* pEvent = dynamic_cast<MenuItemEvent*>(events[i]))
		{
			if (pEvent->IsEnabled())
				m_events.push_back(pEvent);
		}
	}
}

void MenuItemEventsManager::Shutdown()
{
	m_events.clear();
}

std::size_t MenuItemEventsManager::GetNumEvents() const
{
	return m_events.size();
}

MenuItemEvent* MenuItemEventsManager::GetEvent(std::size_t index)
{
	_ASSERTE(index >= 0 && index < m_events.size());
	return m_events[index];
}

void MenuItemEventsManager::OnEventRemoved(Event* pEvent)
{
	auto it = std::find_if(m_events.begin(), m_events.end(), boost::lambda::_1 == pEvent);

	if (it == m_events.end())
		return;

	m_events.erase(it);
}

void MenuItemEventsManager::OnEventUpdated(Event* pEvent)
{
	auto it = std::find_if(m_events.begin(), m_events.end(), boost::lambda::_1 == pEvent);

	if (it == m_events.end())
		return;

	// Remove if it has been disabled.
	if (!(*it)->IsEnabled())
		m_events.erase(it);
}

void MenuItemEventsManager::EmitEvent(std::size_t index)
{
	ServiceManager::Instance().GetRootController().ProcessEvent(GetEvent(index));
}

