#pragma once

#include "s11n_blocks.h"
#include "action_s11n_block.h"

struct ActionListS11nBlock : public S11nBlocks::Block<ActionListS11nBlock>
{
	S11nBlocks::Field<GUID, 1> guid;
	S11nBlocks::Field<pfc::string8, 2> name;
	S11nBlocks::RepeatedField<ActionS11nBlock, 3> actions;
	S11nBlocks::Field<bool, 4> restartAfterCompletion;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(guid)(name)(actions)(restartAfterCompletion);
	}
};