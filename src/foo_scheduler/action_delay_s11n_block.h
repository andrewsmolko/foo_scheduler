#pragma once

#include "s11n_blocks.h"

struct ActionDelayS11nBlock : public S11nBlocks::Block<ActionDelayS11nBlock>
{
	S11nBlocks::Field<int, 1> delay;
	S11nBlocks::Field<int, 2> delayUnits;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(delay)(delayUnits);
	}
};