#pragma once

#include "s11n_blocks.h"

struct ActionStartPlaybackS11nBlock : public S11nBlocks::Block<ActionStartPlaybackS11nBlock>
{
	S11nBlocks::Field<int, 1> playbackStartType;
	S11nBlocks::Field<t_uint32, 2> startTrackNumber;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(playbackStartType)(startTrackNumber);
	}
};