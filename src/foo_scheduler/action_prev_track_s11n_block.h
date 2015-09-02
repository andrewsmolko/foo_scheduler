#pragma once

#include "s11n_blocks.h"

struct ActionPrevTrackS11nBlock : public S11nBlocks::Block<ActionPrevTrackS11nBlock>
{
	template<class Archive>
	void RegisterFields(Archive& ar)
	{
	}
};