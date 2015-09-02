#pragma once

#include "s11n_blocks.h"

struct ActionPausePlaybackS11nBlock : public S11nBlocks::Block<ActionPausePlaybackS11nBlock>
{
	template<class Archive>
	void RegisterFields(Archive& ar)
	{
	}
};