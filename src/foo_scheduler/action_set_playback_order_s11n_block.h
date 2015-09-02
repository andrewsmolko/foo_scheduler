#pragma once

#include "s11n_blocks.h"

struct ActionSetPlaybackOrderS11nBlock : public S11nBlocks::Block<ActionSetPlaybackOrderS11nBlock>
{
	S11nBlocks::Field<GUID, 1> orderGUID;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(orderGUID);
	}
};