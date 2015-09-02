#pragma once

#include "s11n_blocks.h"

struct ActionSetVolumeS11nBlock : public S11nBlocks::Block<ActionSetVolumeS11nBlock>
{
	S11nBlocks::Field<float, 1> volume;
	S11nBlocks::Field<bool, 2> useFade;
	S11nBlocks::Field<int, 3> fadeDuration;
	S11nBlocks::Field<int, 4> fadeDurationUnits;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(volume)(useFade)(fadeDuration)(fadeDurationUnits);
	}
};