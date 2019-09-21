#include "pch.h"
#include "action_stop_playback.h"
#include "service_manager.h"

//------------------------------------------------------------------------------
// ActionStopPlayback
//------------------------------------------------------------------------------

GUID ActionStopPlayback::GetPrototypeGUID() const
{
	// {d81fff42-3a4c-476f-9e1d-c664def51dcf} 
	static const GUID result = 
	{ 0xd81fff42, 0x3a4c, 0x476f, { 0x9e, 0x1d, 0xc6, 0x64, 0xde, 0xf5, 0x1d, 0xcf } };

	return result;
}

int ActionStopPlayback::GetPriority() const
{
	return 10;
}

std::wstring ActionStopPlayback::GetName() const
{
	return L"Stop playback";
}

IAction* ActionStopPlayback::Clone() const
{
	return new ActionStopPlayback(*this);
}

std::wstring ActionStopPlayback::GetDescription() const
{
	return GetName();
}

bool ActionStopPlayback::HasConfigDialog() const
{
	return false;
}

bool ActionStopPlayback::ShowConfigDialog(CWindow parent)
{
	_ASSERTE(false);
	return false;
}

ActionExecSessionPtr ActionStopPlayback::CreateExecSession() const
{
	return ActionExecSessionPtr(new ExecSession(*this));
}

void ActionStopPlayback::LoadFromS11nBlock(const ActionS11nBlock& block)
{

}

void ActionStopPlayback::SaveToS11nBlock(ActionS11nBlock& block) const
{

}

namespace
{
	const bool registered = ServiceManager::Instance().GetActionPrototypesManager().RegisterPrototype(
		new ActionStopPlayback);
}

//------------------------------------------------------------------------------
// ActionStopPlayback::ExecSession
//------------------------------------------------------------------------------

ActionStopPlayback::ExecSession::ExecSession(const ActionStopPlayback& action) : m_action(action)
{
}

void ActionStopPlayback::ExecSession::Run(const AsyncCall::CallbackPtr& completionCall)
{
	static_api_ptr_t<playback_control> pc;

	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(true);
	pc->stop();
	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(false);

	AsyncCall::AsyncRunInMainThread(completionCall);
}

const IAction* ActionStopPlayback::ExecSession::GetParentAction() const
{
	return &m_action;
}

void ActionStopPlayback::ExecSession::Init(IActionListExecSessionFuncs&)
{
}

bool ActionStopPlayback::ExecSession::GetCurrentStateDescription(std::wstring& /*descr*/) const
{
	return false;
}