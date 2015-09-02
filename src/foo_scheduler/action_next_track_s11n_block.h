#pragma once

#include "s11n_blocks.h"

struct ActionNextTrackS11nBlock : public S11nBlocks::Block<ActionNextTrackS11nBlock>
{
	template<class Archive>
	void RegisterFields(Archive& ar)
	{
	}
};