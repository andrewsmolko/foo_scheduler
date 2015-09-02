#include "pch.h"
#include "action_next_track.h"
#include "service_manager.h"

//------------------------------------------------------------------------------
// ActionNextTrack
//------------------------------------------------------------------------------

GUID ActionNextTrack::GetPrototypeGUID() const
{
	// {56108597-C5B4-4808-AD1C-3E637B0E0515}
	static const GUID result = 
	{ 0x56108597, 0xc5b4, 0x4808, { 0xad, 0x1c, 0x3e, 0x63, 0x7b, 0xe, 0x5, 0x15 } };

	return result;
}

int ActionNextTrack::GetPriority() const
{
	return 25;
}

std::wstring ActionNextTrack::GetName() const
{
	return L"Next track";
}

IAction* ActionNextTrack::Clone() const
{
	return new ActionNextTrack(*this);
}

std::wstring ActionNextTrack::GetDescription() const
{
	return GetName();
}

bool ActionNextTrack::HasConfigDialog() const
{
	return false;
}

bool ActionNextTrack::ShowConfigDialog(CWindow parent)
{
	_ASSERTE(false);
	return false;
}

ActionExecSessionPtr ActionNextTrack::CreateExecSession() const
{
	return ActionExecSessionPtr(new ExecSession(*this));
}

void ActionNextTrack::LoadFromS11nBlock(const ActionS11nBlock& block)
{

}

void ActionNextTrack::SaveToS11nBlock(ActionS11nBlock& block) const
{

}

namespace
{
	const bool registered = ServiceManager::Instance().GetActionPrototypesManager().RegisterPrototype(
		new ActionNextTrack);
}

//------------------------------------------------------------------------------
// ActionNextTrack::ExecSession
//------------------------------------------------------------------------------

ActionNextTrack::ExecSession::ExecSession(const ActionNextTrack& action) : m_action(action)
{
}

void ActionNextTrack::ExecSession::Run(const AsyncCall::CallbackPtr& completionCall)
{
	static_api_ptr_t<playback_control> pc;

	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(true);
	pc->start(playback_control::track_command_next);
	ServiceManager::Instance().GetPlayerEventsManager().BlockEvents(false);

	AsyncCall::AsyncRunInMainThread(completionCall);
}

const IAction* ActionNextTrack::ExecSession::GetParentAction() const
{
	return &m_action;
}

void ActionNextTrack::ExecSession::Init(const boost::function<void ()>& /*updateALESDescriptionFunc*/)
{
}

bool ActionNextTrack::ExecSession::GetCurrentStateDescription(std::wstring& /*descr*/) const
{
	return false;
}