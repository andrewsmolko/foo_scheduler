#pragma once

#include "action_list.h"
#include "action_exec_session.h"

class ActionListExecSession : public boost::enable_shared_from_this<ActionListExecSession>, private boost::noncopyable
{
public:
	ActionListExecSession(ActionList* pActionList, const std::wstring& eventDescription);
	~ActionListExecSession();

	void StartExecution();
	
	std::wstring GetDescription() const;

private:
	void RunNextAction();
	void UpdateDescription();

private:
	boost::scoped_ptr<ActionList> m_pActionList;
	std::wstring m_eventDescription;
	int m_currentActionIndex;

	ActionExecSessionPtr m_pActionExecSession;
};

typedef boost::shared_ptr<ActionListExecSession> ActionListExecSessionPtr;

