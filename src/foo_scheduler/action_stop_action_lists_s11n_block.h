#pragma once

#include "s11n_blocks.h"

struct ActionStopActionListsS11nBlock : public S11nBlocks::Block<ActionStopActionListsS11nBlock>
{
	template<class Archive>
	void RegisterFields(Archive& ar)
	{
	}
};