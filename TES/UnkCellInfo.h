#pragma once

#include "../../skse64/GameTypes.h"

namespace TES
{
	struct UnkCellInfo
	{
		UInt16          unk01;
		UInt32          unk04;
		UInt32          unk08;
		UInt16          unk0C;
		UInt8			unk0E;
		UInt64          unk10;
		UInt32          unk18;
		float			unk1C;
		UInt64          unk20;
		UInt32			unk28;
		UInt32			unk2C;
		tArray<UInt32>  actorHandles;	// 30
										// more
	};
}