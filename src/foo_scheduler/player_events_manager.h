#pragma once

#include "player_event.h"

class Model;

class PlayerEventsManager : public play_callback, private boost::noncopyable
{
public:
	explicit PlayerEventsManager(Model& model);

	void Init();
	void Reset();
	void Shutdown();

	void BlockEvents(bool block);

private: // play_callback
	virtual void on_playback_stop(playback_control::t_stop_reason p_reason);
	virtual void on_playback_pause(bool p_state);
	virtual void on_playback_starting(play_control::t_track_command p_command, bool p_paused);
	virtual void on_playback_new_track(metadb_handle_ptr p_track);

	virtual void on_playback_seek(double p_time) {}
	virtual void on_playback_edited(metadb_handle_ptr p_track) {}
	virtual void on_playback_dynamic_info(const file_info & p_info) {}
	virtual void on_playback_dynamic_info_track(const file_info & p_info) {}
	virtual void on_playback_time(double p_time) {}
	virtual void on_volume_change(float p_new_val) {}

private:
	typedef boost::function<bool (PlayerEvent*)> AdditionalConditionFunc;

	void EmitPlayerEvent(PlayerEventType::Type type,
		const AdditionalConditionFunc& condition = AdditionalConditionFunc());

	bool EqualStopReason(playback_control::t_stop_reason reason, PlayerEvent* pEvent);

private:
	bool m_blockEvents;
};