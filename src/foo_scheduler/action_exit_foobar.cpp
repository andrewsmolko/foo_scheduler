#include "pch.h"
#include "action_exit_foobar.h"
#include "service_manager.h"

//------------------------------------------------------------------------------
// ActionExitFoobar
//------------------------------------------------------------------------------

GUID ActionExitFoobar::GetPrototypeGUID() const
{
	// {f52c396c-3c63-49f4-9c15-825d68e94e30} 
	static const GUID result = 
	{ 0xf52c396c, 0x3c63, 0x49f4, { 0x9c, 0x15, 0x82, 0x5d, 0x68, 0xe9, 0x4e, 0x30 } };

	return result;
}

int ActionExitFoobar::GetPriority() const
{
	return 100;
}

std::wstring ActionExitFoobar::GetName() const
{
	return L"Exit foobar2000";
}

IAction* ActionExitFoobar::Clone() const
{
	return new ActionExitFoobar(*this);
}

std::wstring ActionExitFoobar::GetDescription() const
{
	return GetName();
}

bool ActionExitFoobar::HasConfigDialog() const
{
	return false;
}

bool ActionExitFoobar::ShowConfigDialog(CWindow parent)
{
	_ASSERTE(false);
	return false;
}

ActionExecSessionPtr ActionExitFoobar::CreateExecSession() const
{
	return ActionExecSessionPtr(new ExecSession(*this));
}

void ActionExitFoobar::LoadFromS11nBlock(const ActionS11nBlock& block)
{

}

void ActionExitFoobar::SaveToS11nBlock(ActionS11nBlock& block) const
{

}

namespace
{
	const bool registered = ServiceManager::Instance().GetActionPrototypesManager().RegisterPrototype(
		new ActionExitFoobar);
}

//------------------------------------------------------------------------------
// ActionExitFoobar::ExecSession
//------------------------------------------------------------------------------

ActionExitFoobar::ExecSession::ExecSession(const ActionExitFoobar& action) : m_action(action)
{
}

void ActionExitFoobar::ExecSession::Run(const AsyncCall::CallbackPtr& completionCall)
{
	standard_commands::main_exit();

	AsyncCall::AsyncRunInMainThread(completionCall);
}

const IAction* ActionExitFoobar::ExecSession::GetParentAction() const
{
	return &m_action;
}

void ActionExitFoobar::ExecSession::Init(IActionListExecSessionFuncs&)
{
}

bool ActionExitFoobar::ExecSession::GetCurrentStateDescription(std::wstring& /*descr*/) const
{
	return false;
}