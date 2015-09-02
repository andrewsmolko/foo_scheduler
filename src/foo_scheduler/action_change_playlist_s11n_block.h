#pragma once

#include "s11n_blocks.h"

struct ActionChangePlaylistS11nBlock : public S11nBlocks::Block<ActionChangePlaylistS11nBlock>
{
	S11nBlocks::Field<int, 1> changeType;
	S11nBlocks::Field<pfc::string8, 2> playlist;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(changeType)(playlist);
	}
};