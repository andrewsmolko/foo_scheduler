#include "pch.h"
#include "action_prev_track.h"
#include "service_manager.h"

//------------------------------------------------------------------------------
// ActionPrevTrack
//------------------------------------------------------------------------------

GUID ActionPrevTrack::GetPrototypeGUID() const
{
	// {8D5F8432-DB8D-4572-A07D-B06EEE1F7BEF}
	static const GUID result = 
	{ 0x8d5f8432, 0xdb8d, 0x4572, { 0xa0, 0x7d, 0xb0, 0x6e, 0xee, 0x1f, 0x7b, 0xef } };

	return result;
}

int ActionPrevTrack::GetPriority() const
{
	return 26;
}

std::wstring ActionPrevTrack::GetName() const
{
	return L"Previous track";
}

IAction* ActionPrevTrack::Clone() const
{
	return new ActionPrevTrack(*this);
}

std::wstring ActionPrevTrack::GetDescription() const
{
	return GetName();
}

bool ActionPrevTrack::HasConfigDialog() const
{
	return false;
}

bool ActionPrevTrack::ShowConfigDialog(CWindow parent)
{
	_ASSERTE(false);
	return false;
}

ActionExecSessionPtr ActionPrevTrack::CreateExecSession() const
{
	return ActionExecSessionPtr(new ExecSession(*this));
}

void ActionPrevTrack::LoadFromS11nBlock(const ActionS11nBlock& block)
{

}

void ActionPrevTrack::SaveToS11nBlock(ActionS11nBlock& block) const
{

}

namespace
{
	const bool registered = ServiceManager::Instance().GetActionPrototypesManager().RegisterPrototype(
		new ActionPrevTrack);
}

//------------------------------------------------------------------------------
// ActionPrevTrack::ExecSession
//------------------------------------------------------------------------------

ActionPrevTrack::ExecSession::ExecSession(const ActionPrevTrack& action) : m_action(action)
{
}

void ActionPrevTrack::ExecSession::Run(const AsyncCall::CallbackPtr& completionCall)
{
	static_api_ptr_t<playback_control> pc;

	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(true);
	pc->start(playback_control::track_command_prev);
	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(false);

	AsyncCall::AsyncRunInMainThread(completionCall);
}

const IAction* ActionPrevTrack::ExecSession::GetParentAction() const
{
	return &m_action;
}

void ActionPrevTrack::ExecSession::Init(IActionListExecSessionFuncs&)
{
}

bool ActionPrevTrack::ExecSession::GetCurrentStateDescription(std::wstring& /*descr*/) const
{
	return false;
}