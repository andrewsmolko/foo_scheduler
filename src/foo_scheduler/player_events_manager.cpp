#include "pch.h"
#include "player_events_manager.h"
#include "service_manager.h"

PlayerEventsManager::PlayerEventsManager(Model& model) : m_blockEvents(false)
{
	model.ConnectModelStateChangedSlot(
		boost::bind(&PlayerEventsManager::Reset, this));
}

void PlayerEventsManager::Init()
{
	
}

void PlayerEventsManager::Reset()
{
	static_api_ptr_t<play_callback_manager>()->unregister_callback(this);

	if (!ServiceManager::Instance().GetModel().IsSchedulerEnabled())
		return;

	static_api_ptr_t<play_callback_manager>()->register_callback(this, flag_on_playback_all, false);
}

void PlayerEventsManager::Shutdown()
{
	static_api_ptr_t<play_callback_manager>()->unregister_callback(this);
}

void PlayerEventsManager::BlockEvents(bool block)
{
	m_blockEvents = block;
}

void PlayerEventsManager::EmitPlayerEvent(PlayerEventType::Type type,
	const AdditionalConditionFunc& condition)
{
	if (m_blockEvents)
		return;

	std::vector<Event*> events = ServiceManager::Instance().GetModel().GetEvents();

	for (std::size_t i = 0; i < events.size(); ++i)
	{
		if (PlayerEvent* pEvent = dynamic_cast<PlayerEvent*>(events[i]))
		{
			if (pEvent->IsEnabled() && pEvent->GetType() == type)
			{
				if (condition.empty() || condition(pEvent))
					ServiceManager::Instance().GetRootController().ProcessEvent(pEvent);
			}
		}
	}
}

bool PlayerEventsManager::EqualStopReason(playback_control::t_stop_reason reason, PlayerEvent* pEvent)
{
	PlayerEventStopReason::Type stopReason = PlayerEventStopReason::user;

	switch (reason)
	{
	case playback_control::stop_reason_user:
		stopReason = PlayerEventStopReason::user;
		break;

	case playback_control::stop_reason_eof:
		stopReason = PlayerEventStopReason::eof;
		break;

	case playback_control::stop_reason_starting_another:
	case playback_control::stop_reason_shutting_down:
		_ASSERTE(false);
		break;
	}

	const PlayerEvent::StopReasons& sr = pEvent->GetStopReasons();
	auto it = std::find(sr.cbegin(), sr.cend(), stopReason);
	
	return it != sr.end();
}

void PlayerEventsManager::on_playback_stop(playback_control::t_stop_reason reason)
{
	// Ignore this reason cause we're shutting down and
	// nothing can stay alive after the end of a world! :)
	if (reason == playback_control::stop_reason_shutting_down)
		return;

	EmitPlayerEvent(PlayerEventType::onPlaybackStop,
		boost::bind(&PlayerEventsManager::EqualStopReason, this, reason, _1));
}

void PlayerEventsManager::on_playback_pause(bool state)
{
	EmitPlayerEvent(state ? PlayerEventType::onPlaybackPause : PlayerEventType::onPlaybackUnpause);
}

void PlayerEventsManager::on_playback_starting(play_control::t_track_command p_command, bool p_paused)
{
	EmitPlayerEvent(PlayerEventType::onPlaybackStart);
}

void PlayerEventsManager::on_playback_new_track(metadb_handle_ptr p_track)
{
	EmitPlayerEvent(PlayerEventType::onPlaybackNewTrack);
}

