#pragma once

#include "s11n_blocks.h"

struct ActionStopPlaybackS11nBlock : public S11nBlocks::Block<ActionStopPlaybackS11nBlock>
{
	template<class Archive>
	void RegisterFields(Archive& ar)
	{
	}
};