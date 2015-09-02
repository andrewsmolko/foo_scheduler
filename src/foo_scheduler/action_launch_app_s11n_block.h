#pragma once

#include "s11n_blocks.h"

struct ActionLaunchAppS11nBlock : public S11nBlocks::Block<ActionLaunchAppS11nBlock>
{
	S11nBlocks::Field<pfc::string8, 1> commandLine;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(commandLine);
	}
};