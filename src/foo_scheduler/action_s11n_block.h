#pragma once

#include "s11n_blocks.h"

#include "action_start_playback_s11n_block.h"
#include "action_stop_playback_s11n_block.h"
#include "action_pause_playback_s11n_block.h"
#include "action_exit_foobar_s11n_block.h"
#include "action_shutdown_s11n_block.h"
#include "action_change_playlist_s11n_block.h"
#include "action_set_playback_order_s11n_block.h"
#include "action_delay_s11n_block.h"
#include "action_set_volume_s11n_block.h"
#include "action_launch_app_s11n_block.h"
#include "action_mute_s11n_block.h"
#include "action_next_track_s11n_block.h"
#include "action_prev_track_s11n_block.h"
#include "action_wait_n_tracks_played_s11n_block.h"
#include "action_save_playback_state_s11n_block.h"

struct ActionS11nBlock : public S11nBlocks::Block<ActionS11nBlock>
{
	S11nBlocks::Field<GUID, 1, true> actionGUID; // Required field.

	S11nBlocks::Field<ActionStartPlaybackS11nBlock, 2>    startPlayback;
	S11nBlocks::Field<ActionStopPlaybackS11nBlock, 3>     stopPlayback;
	S11nBlocks::Field<ActionPausePlaybackS11nBlock, 4>    pausePlayback;
	S11nBlocks::Field<ActionExitFoobarS11nBlock, 5>       exitFoobar;
	S11nBlocks::Field<ActionShutdownS11nBlock, 6>         shutdown;
	S11nBlocks::Field<ActionChangePlaylistS11nBlock, 7>   changePlaylist;
	S11nBlocks::Field<ActionSetPlaybackOrderS11nBlock, 8> setPlaybackOrder;
	S11nBlocks::Field<ActionDelayS11nBlock, 9>            delay;
	S11nBlocks::Field<ActionSetVolumeS11nBlock, 10>       setVolume;
	S11nBlocks::Field<ActionLaunchAppS11nBlock, 11>       launchApp;
	S11nBlocks::Field<ActionToggleMuteS11nBlock, 12>      toggleMute;
	S11nBlocks::Field<ActionNextTrackS11nBlock, 13>       nextTrack;
	S11nBlocks::Field<ActionPrevTrackS11nBlock, 14>       prevTrack;
	S11nBlocks::Field<ActionWaitNTracksPlayedS11nBlock, 15> waitNTracksPlayed;
    S11nBlocks::Field<ActionSavePlaybackStateS11nBlock, 16> savePlaybackState;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
        ar.RegisterFields(actionGUID)(startPlayback)(stopPlayback)(pausePlayback)
            (exitFoobar)(shutdown)(changePlaylist)(setPlaybackOrder)(delay)(setVolume)(launchApp)
            (toggleMute)(nextTrack)(prevTrack)(waitNTracksPlayed)(savePlaybackState);
	}
};