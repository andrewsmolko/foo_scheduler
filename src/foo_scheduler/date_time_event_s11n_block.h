#pragma once

#include "s11n_blocks.h"

struct DateTimeEventS11nBlock : public S11nBlocks::Block<DateTimeEventS11nBlock>
{
	S11nBlocks::Field<pfc::string8, 1> title;
	S11nBlocks::Field<int, 2> type;
	S11nBlocks::Field<pfc::string8, 3> date;
	S11nBlocks::Field<char, 4> weekDays;
	S11nBlocks::Field<pfc::string8, 5> time;
	S11nBlocks::Field<bool, 6> wakeup;
	S11nBlocks::Field<int, 7> finalAction;

	template<class Archive>
	void RegisterFields(Archive& ar)
	{
		ar.RegisterFields(title)(type)(date)(weekDays)(time)(wakeup)(finalAction);
	}
};