#pragma once

#include "s11n_blocks.h"

struct ActionExitFoobarS11nBlock : public S11nBlocks::Block<ActionExitFoobarS11nBlock>
{
	template<class Archive>
	void RegisterFields(Archive& ar)
	{
	}
};