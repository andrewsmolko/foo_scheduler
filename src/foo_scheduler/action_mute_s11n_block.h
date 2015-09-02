#pragma once

#include "s11n_blocks.h"

struct ActionToggleMuteS11nBlock : public S11nBlocks::Block<ActionToggleMuteS11nBlock>
{
	template<class Archive>
	void RegisterFields(Archive& ar)
	{
	}
};