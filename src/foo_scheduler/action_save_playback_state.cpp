#include "pch.h"
#include "action_save_playback_state.h"
#include "service_manager.h"
#include "scope_exit_function.h"

const char* ActionSavePlaybackState::GetPlaylistKey()
{
    return "saved_playback_state/playlist";
}

const char* ActionSavePlaybackState::GetTrackKey()
{
    return "saved_playback_state/track";
}

const char * ActionSavePlaybackState::GetPositionKey()
{
	return "saved_playback_state/position";
}

//------------------------------------------------------------------------------
// ActionSavePlaybackState
//------------------------------------------------------------------------------

GUID ActionSavePlaybackState::GetPrototypeGUID() const
{
    // {095fb449-74ef-4913-8ab3-267088de810c} 
    static const GUID result = 
    { 0x095fb449, 0x74ef, 0x4913, { 0x8a, 0xb3, 0x26, 0x70, 0x88, 0xde, 0x81, 0x0c } };

    return result;
}

int ActionSavePlaybackState::GetPriority() const
{
    return 45;
}

std::wstring ActionSavePlaybackState::GetName() const
{
    return L"Save playback state";
}

IAction* ActionSavePlaybackState::Clone() const
{
    return new ActionSavePlaybackState(*this);
}

std::wstring ActionSavePlaybackState::GetDescription() const
{
    return GetName();
}

bool ActionSavePlaybackState::HasConfigDialog() const
{
    return false;
}

bool ActionSavePlaybackState::ShowConfigDialog(CWindow parent)
{
    _ASSERTE(false);
    return false;
}

ActionExecSessionPtr ActionSavePlaybackState::CreateExecSession() const
{
    return ActionExecSessionPtr(new ExecSession(*this));
}

void ActionSavePlaybackState::LoadFromS11nBlock(const ActionS11nBlock& /*block*/)
{
}

void ActionSavePlaybackState::SaveToS11nBlock(ActionS11nBlock& /*block*/) const
{
}

namespace
{
    const bool registered = ServiceManager::Instance().GetActionPrototypesManager().RegisterPrototype(
        new ActionSavePlaybackState);
}

//------------------------------------------------------------------------------
// ActionSavePlaybackState::ExecSession
//------------------------------------------------------------------------------

ActionSavePlaybackState::ExecSession::ExecSession(const ActionSavePlaybackState& action) : m_action(action)
{
}

void ActionSavePlaybackState::ExecSession::Run(const AsyncCall::CallbackPtr& completionCall)
{
    ScopeExitFunction scopeExit(boost::bind(&AsyncCall::AsyncRunInMainThread, completionCall));

    // Erase previous state from action list exec session.
    m_alesFuncs->SetValue(GetPlaylistKey(), boost::any());
    m_alesFuncs->SetValue(GetTrackKey(), boost::any());

    static_api_ptr_t<playlist_manager> pm;
    t_size playlist = -1;
    t_size track = -1;
    if (!pm->get_playing_item_location(&playlist, &track))
        return;

    m_alesFuncs->SetValue(GetPlaylistKey(), playlist);
    m_alesFuncs->SetValue(GetTrackKey(), track);
	m_alesFuncs->SetValue(GetPositionKey(), static_api_ptr_t<playback_control>()->playback_get_position());
}

const IAction* ActionSavePlaybackState::ExecSession::GetParentAction() const
{
    return &m_action;
}

void ActionSavePlaybackState::ExecSession::Init(IActionListExecSessionFuncs& alesFuncs)
{
    m_alesFuncs = &alesFuncs;
}

bool ActionSavePlaybackState::ExecSession::GetCurrentStateDescription(std::wstring& /*descr*/) const
{
    return false;
}