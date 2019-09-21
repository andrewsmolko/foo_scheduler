#pragma once

#include "action_list.h"
#include "action_exec_session.h"

class ActionListExecSession
    : public boost::enable_shared_from_this<ActionListExecSession>
    , private boost::noncopyable
    , public IActionListExecSessionFuncs
{
public:
	ActionListExecSession(ActionList* pActionList, const std::wstring& eventDescription);
	~ActionListExecSession();

	void StartExecution();
	
	std::wstring GetDescription() const;

private: // ActionListKeyValueStore
    boost::any GetValue(const std::string &key) const override;
    void SetValue(const std::string &key, const boost::any &value) override;
    void UpdateDescription() override;

private:
	void RunNextAction();

private:
	boost::scoped_ptr<ActionList> m_pActionList;
	std::wstring m_eventDescription;
	int m_currentActionIndex;

	ActionExecSessionPtr m_pActionExecSession;
    std::map<std::string, boost::any> m_keyValueStore;
};

typedef boost::shared_ptr<ActionListExecSession> ActionListExecSessionPtr;

