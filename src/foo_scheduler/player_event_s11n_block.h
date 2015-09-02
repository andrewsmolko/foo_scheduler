#pragma once

#include "s11n_blocks.h"

struct PlayerEventS11nBlock : public S11nBlocks::Block<PlayerEventS11nBlock>
{
	S11nBlocks::Field<int, 1> type;
	S11nBlocks::Field<int, 2> finalAction;
	S11nBlocks::RepeatedField<int, 3> stopReasons;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(type)(finalAction)(stopReasons);
	}
};