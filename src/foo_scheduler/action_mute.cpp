#include "pch.h"
#include "action_mute.h"
#include "service_manager.h"

//------------------------------------------------------------------------------
// ActionToggleMute
//------------------------------------------------------------------------------

GUID ActionToggleMute::GetPrototypeGUID() const
{
	// {95cd123f-0ae6-4baa-ac51-a2c410f0b2dd} 
	static const GUID result = 
	{ 0x95cd123f, 0x0ae6, 0x4baa, { 0xac, 0x51, 0xa2, 0xc4, 0x10, 0xf0, 0xb2, 0xdd } };

	return result;
}

int ActionToggleMute::GetPriority() const
{
	return 36;
}

std::wstring ActionToggleMute::GetName() const
{
	return L"Mute/unmute";
}

IAction* ActionToggleMute::Clone() const
{
	return new ActionToggleMute(*this);
}

std::wstring ActionToggleMute::GetDescription() const
{
	return GetName();
}

bool ActionToggleMute::HasConfigDialog() const
{
	return false;
}

bool ActionToggleMute::ShowConfigDialog(CWindow parent)
{
	_ASSERTE(false);
	return false;
}

ActionExecSessionPtr ActionToggleMute::CreateExecSession() const
{
	return ActionExecSessionPtr(new ExecSession(*this));
}

void ActionToggleMute::LoadFromS11nBlock(const ActionS11nBlock& block)
{

}

void ActionToggleMute::SaveToS11nBlock(ActionS11nBlock& block) const
{

}

namespace
{
	const bool registered = ServiceManager::Instance().GetActionPrototypesManager().RegisterPrototype(
		new ActionToggleMute);
}

//------------------------------------------------------------------------------
// ActionToggleMute::ExecSession
//------------------------------------------------------------------------------

ActionToggleMute::ExecSession::ExecSession(const ActionToggleMute& action) : m_action(action)
{

}

const IAction* ActionToggleMute::ExecSession::GetParentAction() const
{
	return &m_action;
}

void ActionToggleMute::ExecSession::Run(const AsyncCall::CallbackPtr& completionCall)
{
	static_api_ptr_t<playback_control> pc;
	pc->volume_mute_toggle();

	AsyncCall::AsyncRunInMainThread(completionCall);
}

void ActionToggleMute::ExecSession::Init(IActionListExecSessionFuncs&)
{
}

bool ActionToggleMute::ExecSession::GetCurrentStateDescription(std::wstring& /*descr*/) const
{
	return false;
}