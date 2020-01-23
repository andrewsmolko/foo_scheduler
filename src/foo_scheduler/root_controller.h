#pragma once

#include "event.h"
#include "action_list_exec_session.h"

class DateTimeEvent;
class MenuItemEvent;
class StatusWindow;

class RootController : private boost::noncopyable
{
public:
	RootController();

	// Initializes plugin and starts event processing if necessary.
	void Init();
	void Shutdown();

	void ShowPreferencesPage();
	void ShowStatusWindow();

	void ProcessEvent(Event* pEvent);

	void UpdateExecSession(const ActionListExecSessionPtr& pSession);
	void RemoveExecSession(ActionListExecSession* pSession);
	void RemoveAllExecSessions();
	void RemoveAllExecSessionsBut(ActionListExecSession* session);
	std::vector<ActionListExecSession*> GetActionListExecSessions();

	typedef boost::signals2::signal<void (ActionListExecSession*)> ActionListExecSessionAdded;
	typedef boost::signals2::signal<void (ActionListExecSession*)> ActionListExecSessionRemoved;
	typedef boost::signals2::signal<void (ActionListExecSession*)> ActionListExecSessionUpdated;

	boost::signals2::connection ConnectActionListExecSessionAddedSlot(
		const ActionListExecSessionAdded::slot_type& slot);

	boost::signals2::connection ConnectActionListExecSessionRemovedSlot(
		const ActionListExecSessionRemoved::slot_type& slot);

	boost::signals2::connection ConnectActionListExecSessionUpdatedSlot(
		const ActionListExecSessionUpdated::slot_type& slot);

private:
	void StopExecutionSessions();
	void ClearStatusWindowPtr();

private:
	std::vector<ActionListExecSessionPtr> m_execSessions;
	StatusWindow* m_pStatusWindow;

	ActionListExecSessionAdded m_actionListExecSessionAddedSignal;
	ActionListExecSessionRemoved m_actionListExecSessionRemovedSignal;
	ActionListExecSessionUpdated m_actionListExecSessionUpdatedSignal;
};