#include "pch.h"
#include "root_controller.h"
#include "service_manager.h"
#include "pref_page.h"
#include "status_window.h"

RootController::RootController() : m_pStatusWindow(0)
{

}

void RootController::Init()
{
	ServiceManager::Instance().GetTimersManager().Init();
	ServiceManager::Instance().GetPlayerEventsManager().Init();

	ServiceManager::Instance().GetPlayerEventsManager().Reset();
	ServiceManager::Instance().GetMenuItemEventsManager().Reset();
	ServiceManager::Instance().GetDateTimeEventsManager().Reset();
}

void RootController::Shutdown()
{
	ServiceManager::Instance().GetPlayerEventsManager().Shutdown();
	ServiceManager::Instance().GetMenuItemEventsManager().Shutdown();
	ServiceManager::Instance().GetDateTimeEventsManager().Shutdown();

	StopExecutionSessions();

	ServiceManager::Instance().GetTimersManager().Shutdown();
}

void RootController::ShowPreferencesPage()
{
	static_api_ptr_t<ui_control> uc;
	uc->show_preferences(PreferencesPageImpl::m_guid);
}

void RootController::ProcessEvent(Event* pEvent)
{
	ActionList* pActionList = ServiceManager::Instance().GetModel().GetActionListByGUID(
		pEvent->GetActionListGUID());

	std::wstring eventDescription = pEvent->GetDescription();

	// After calling Event::OnSignal don't call Event's functions, cause it might have been deleted
	// in OnSignal.
	pEvent->OnSignal();
	
	if (!pActionList)
		return;

	ActionListExecSessionPtr pSession(new ActionListExecSession(pActionList, eventDescription));
	m_execSessions.push_back(pSession);

	m_actionListExecSessionAddedSignal(pSession.get());

	pSession->StartExecution();
}

void RootController::RemoveExecSession(ActionListExecSession* pSession)
{
	for (std::size_t i = 0; i < m_execSessions.size(); ++i)
		if (m_execSessions[i].get() == pSession)
		{
			ActionListExecSessionPtr pExecSession = m_execSessions[i];
			m_execSessions.erase(m_execSessions.begin() + i);

			m_actionListExecSessionRemovedSignal(pExecSession.get());
			return;
		}
}

void RootController::RemoveAllExecSessions()
{
	// Make a copy.
	std::vector<ActionListExecSessionPtr> execSessions = m_execSessions;

	// Clear the model.
	m_execSessions.clear();

	// Notify about removal of each session.
	for (std::size_t i = 0; i < execSessions.size(); ++i)
		m_actionListExecSessionRemovedSignal(execSessions[i].get());

	// Upon scope exit execSessions will be deleted and all sessions will be stopped.
}

void RootController::StopExecutionSessions()
{
	m_execSessions.clear();
}

void RootController::ShowStatusWindow()
{
	if (m_pStatusWindow)
		m_pStatusWindow->Activate();
	else
	{
		m_pStatusWindow = new StatusWindow(core_api::get_main_window(),
			boost::bind(&RootController::ClearStatusWindowPtr, this));
	}
}

void RootController::ClearStatusWindowPtr()
{
	m_pStatusWindow = 0;
}

std::vector<ActionListExecSession*> RootController::GetActionListExecSessions()
{
	std::vector<ActionListExecSession*> result;

	for (std::size_t i = 0; i < m_execSessions.size(); ++i)
		result.push_back(m_execSessions[i].get());

	return result;
}

boost::signals2::connection RootController::ConnectActionListExecSessionRemovedSlot(
	const ActionListExecSessionRemoved::slot_type& slot)
{
	return m_actionListExecSessionRemovedSignal.connect(slot);
}

boost::signals2::connection RootController::ConnectActionListExecSessionUpdatedSlot(
	const ActionListExecSessionUpdated::slot_type& slot)
{
	return m_actionListExecSessionUpdatedSignal.connect(slot);
}

boost::signals2::connection RootController::ConnectActionListExecSessionAddedSlot(
	const ActionListExecSessionAdded::slot_type& slot)
{
	return m_actionListExecSessionAddedSignal.connect(slot);
}

void RootController::UpdateExecSession(const ActionListExecSessionPtr& pSession)
{
	m_actionListExecSessionUpdatedSignal(pSession.get());
}
