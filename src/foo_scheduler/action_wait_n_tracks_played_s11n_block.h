#pragma once

#include "s11n_blocks.h"

struct ActionWaitNTracksPlayedS11nBlock : public S11nBlocks::Block<ActionWaitNTracksPlayedS11nBlock>
{
	S11nBlocks::Field<int, 1> numTracks;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(numTracks);
	}
};