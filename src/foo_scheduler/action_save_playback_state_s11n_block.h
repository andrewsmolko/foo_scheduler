#pragma once

#include "s11n_blocks.h"

struct ActionSavePlaybackStateS11nBlock : public S11nBlocks::Block<ActionSavePlaybackStateS11nBlock>
{
    template<class Archive>
    void RegisterFields(Archive& ar)
    {
    }
};