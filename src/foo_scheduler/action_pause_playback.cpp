#include "pch.h"
#include "action_pause_playback.h"
#include "service_manager.h"

//------------------------------------------------------------------------------
// ActionPausePlayback
//------------------------------------------------------------------------------

GUID ActionPausePlayback::GetPrototypeGUID() const
{
	// {b280ccda-6573-4be6-b77d-cd12d9474af2} 
	static const GUID result = 
	{ 0xb280ccda, 0x6573, 0x4be6, { 0xb7, 0x7d, 0xcd, 0x12, 0xd9, 0x47, 0x4a, 0xf2 } };

	return result;
}

int ActionPausePlayback::GetPriority() const
{
	return 20;
}

std::wstring ActionPausePlayback::GetName() const
{
	return L"Pause/unpause playback";
}

IAction* ActionPausePlayback::Clone() const
{
	return new ActionPausePlayback(*this);
}

std::wstring ActionPausePlayback::GetDescription() const
{
	return GetName();
}

bool ActionPausePlayback::HasConfigDialog() const
{
	return false;
}

bool ActionPausePlayback::ShowConfigDialog(CWindow parent)
{
	_ASSERTE(false);
	return false;
}

ActionExecSessionPtr ActionPausePlayback::CreateExecSession() const
{
	return ActionExecSessionPtr(new ExecSession(*this));
}

void ActionPausePlayback::LoadFromS11nBlock(const ActionS11nBlock& block)
{

}

void ActionPausePlayback::SaveToS11nBlock(ActionS11nBlock& block) const
{

}

namespace
{
	const bool registered = ServiceManager::Instance().GetActionPrototypesManager().RegisterPrototype(
		new ActionPausePlayback);
}

//------------------------------------------------------------------------------
// ActionPausePlayback::ExecSession
//------------------------------------------------------------------------------

ActionPausePlayback::ExecSession::ExecSession(const ActionPausePlayback& action) : m_action(action)
{
}

void ActionPausePlayback::ExecSession::Run(const AsyncCall::CallbackPtr& completionCall)
{
	static_api_ptr_t<playback_control> pc;

	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(true);
	pc->play_or_pause();
	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(false);

	AsyncCall::AsyncRunInMainThread(completionCall);
}

const IAction* ActionPausePlayback::ExecSession::GetParentAction() const
{
	return &m_action;
}

void ActionPausePlayback::ExecSession::Init(const boost::function<void ()>& /*updateALESDescriptionFunc*/)
{
}

bool ActionPausePlayback::ExecSession::GetCurrentStateDescription(std::wstring& /*descr*/) const
{
	return false;
}