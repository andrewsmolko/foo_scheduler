#include "pch.h"
#include "action_list_exec_session.h"
#include "async_call.h"
#include "service_manager.h"
#include "action_list.h"

ActionListExecSession::ActionListExecSession(ActionList* pActionList, const std::wstring& eventDescription) :
	m_pActionList(pActionList->Clone()), m_eventDescription(eventDescription), m_currentActionIndex(-1)
{
	// pActionList is cloned as the following situation may occur:
	// an action list is running and this session references it. User removes or modifies the action list and
	// after applying new settings this session will be broken.
}

ActionListExecSession::~ActionListExecSession()
{
	m_pActionExecSession.reset();
}

void ActionListExecSession::StartExecution()
{
	AsyncCall::CallbackPtr callback = AsyncCall::MakeCallback<ActionListExecSession>(
		shared_from_this(), boost::mem_fn(&ActionListExecSession::RunNextAction));

	AsyncCall::AsyncRunInMainThread(callback);
}

void ActionListExecSession::RunNextAction()
{
	++m_currentActionIndex;

	std::vector<IAction*> actions = m_pActionList->GetActions();

	// If no more actions to execute finish this session.
	if (m_currentActionIndex >= static_cast<int>(actions.size()))
	{
		m_pActionExecSession.reset();

		// The last reference to this instance holds AsyncCall::MainThreadAsyncMethodCall::operator ().
		ServiceManager::Instance().GetRootController().RemoveExecSession(this);
		return;
	}

	m_pActionExecSession = actions[m_currentActionIndex]->CreateExecSession();
	_ASSERTE(m_pActionExecSession);

	m_pActionExecSession->Init(*this);

	// Async call takes boost::weak_ptr, which is automatically constructed from boost::shared_ptr.
	AsyncCall::CallbackPtr runNextActionCallback = AsyncCall::MakeCallback<ActionListExecSession>(
		shared_from_this(), boost::mem_fn(&ActionListExecSession::RunNextAction));

	// An action must initiate ActionListExecSession::RunNextAction, not an action list itself,
	// cause there are some continuous actions like Delay or Volume with fade out.
	m_pActionExecSession->Run(runNextActionCallback);

	// Notify everyone that a new action is going to be executed.
	UpdateDescription();
}

std::wstring ActionListExecSession::GetDescription() const
{
	_ASSERTE(m_pActionList);
	std::wstring result = m_pActionList->GetDescription();

	if (m_pActionExecSession)
	{
		result += L" / ";
		result += m_pActionExecSession->GetParentAction()->GetDescription();

		std::wstring stateDescr;
		if (m_pActionExecSession->GetCurrentStateDescription(stateDescr))
		{
			result += L" [";
			result += stateDescr;
			result += L"]";
		}
	}

	return result;
}

void ActionListExecSession::UpdateDescription()
{
	ServiceManager::Instance().GetRootController().UpdateExecSession(shared_from_this());
}

boost::any ActionListExecSession::GetValue(const std::string &key) const
{
    auto it = m_keyValueStore.find(key);
    if (it != m_keyValueStore.cend())
    {
        return it->second;
    }
    return boost::any();
}

void ActionListExecSession::SetValue(const std::string &key, const boost::any &value)
{
    if (value.empty())
    {
        m_keyValueStore.erase(key);
    }
    else
    {
        m_keyValueStore[key] = value;
    }
}
