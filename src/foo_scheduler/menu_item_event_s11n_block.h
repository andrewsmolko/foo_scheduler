#pragma once

#include "s11n_blocks.h"

struct MenuItemEventS11nBlock : public S11nBlocks::Block<MenuItemEventS11nBlock>
{
	S11nBlocks::Field<pfc::string8, 1> menuItemName;
	S11nBlocks::Field<int, 2> finalAction;
	S11nBlocks::Field<GUID, 3> guid;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(menuItemName)(finalAction)(guid);
	}
};