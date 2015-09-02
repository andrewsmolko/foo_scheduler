#pragma once

#include "s11n_blocks.h"

struct ActionShutdownS11nBlock : public S11nBlocks::Block<ActionShutdownS11nBlock>
{
	S11nBlocks::Field<int, 1> shutdownType;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(shutdownType);
	}
};