#pragma once

#include "../../skse64/GameEvents.h"

namespace TES
{
	// there's some errors in this list but I can't be bothered to fix them right now (errors exist in skse)
	// combatevent and hitevent are correct and that's what we need
	class EventDispatcherList
	{
	public:
		EventDispatcher<void>								unk00;					//	00
		EventDispatcher<void>								unk58;					//  58  - sink offset 010
		EventDispatcher<TESActiveEffectApplyRemoveEvent>	unkB0;					//  B0  - sink offset 018
		EventDispatcher<void>								unk108;					//  108 - sink offset 020
		EventDispatcher<void>								unk160;					//  160 - sink offset 028
		EventDispatcher<TESCellAttachDetachEvent>			unk1B8;					//  1B8 - sink offset 030
		EventDispatcher<void>								unk210;					//  210 - sink offset 038
		EventDispatcher<void>								unk2C0;					//  2C0 - sink offset 040
		EventDispatcher<TESCombatEvent>						combatDispatcher;		//  318 - sink offset 048
		EventDispatcher<TESContainerChangedEvent>			unk370;					//  370 - sink offset 050
		EventDispatcher<TESDeathEvent>						deathDispatcher;		//  3C8 - sink offset 058
		EventDispatcher<void>								unk420;					//  420 - sink offset 068
		EventDispatcher<void>								unk478;					//  478 - sink offset 070
		EventDispatcher<void>								unk4D0;					//  4D0 - sink offset 078
		EventDispatcher<void>								unk528;					//  528 - sink offset 080
		EventDispatcher<void>								unk580;					//  580 - sink offset 088
		EventDispatcher<void>								unk5D8;					//  5D8 - sink offset 090
		EventDispatcher<TESHitEvent>						hitDispatcher;			//  630 - sink offset 098
		EventDispatcher<TESInitScriptEvent>					initScriptDispatcher;	//  688 - sink offset 0A0
		EventDispatcher<void>								unk6E0;					//  6E0 - sink offset 0A8
		EventDispatcher<void>								unk738;					//  738 - sink offset 0B0
		EventDispatcher<void>								unk790;					//  790 - sink offset 0B8
		EventDispatcher<void>								unk7E8;					//  7E8 - sink offset 0C0
		EventDispatcher<void>								unk840;					//  840 - sink offset 0C8
		EventDispatcher<TESObjectLoadedEvent>				objectLoadedDispatcher;	//  898 - sink offset 0D0
		EventDispatcher<void>								unk8F0;					//  8F0 - sink offset 0D8
		EventDispatcher<void>								unk948;					//  948 - sink offset 0E0
		EventDispatcher<void>								unk9A0;					//  9A0 - sink offset 0E8
		EventDispatcher<void>								unk9F8;					//  9F8 - sink offset 0F0
		EventDispatcher<void>								unkA50;					//  A50 - sink offset 0F8
		EventDispatcher<void>								unkAA8;					//  AA8 - sink offset 100
		EventDispatcher<void>								unkB00;					//  B00 - sink offset 108
		EventDispatcher<void>								unkB58;					//  B58 - sink offset 110
		EventDispatcher<void>								unkBB0;					//  BB0 - sink offset 118
		EventDispatcher<void>								unkC08;					//  C08 - sink offset 120
		EventDispatcher<void>								unkC60;					//  C60 - sink offset 128
		EventDispatcher<void>								unkCB8;					//  CB8 - sink offset 130
		EventDispatcher<void>								unkD10;					//  D10 - sink offset 138
		EventDispatcher<void>								unkD68;					//  D68 - sink offset 140
		EventDispatcher<void>								unkDC0;					//  DC0 - sink offset 148
		EventDispatcher<void>								unkE18;					//  E18 - sink offset 150
		EventDispatcher<void>								unkE70;					//  E70 - sink offset 158
		EventDispatcher<void>								unkEC8;					//  EC8 - sink offset 160
		EventDispatcher<void>								unkF20;					//  F20 - sink offset 168
		EventDispatcher<void>								unkF78;					//  F78 - sink offset 170
		EventDispatcher<void>								unkFD0;					//  FD0 - sink offset 178
		EventDispatcher<void>								unk1028;				// 1028 - sink offset 180
		EventDispatcher<void>								unk1080;				// 1080 - sink offset 188
		EventDispatcher<void>								unk10D8;				// 10D8 - sink offset 190
		EventDispatcher<void>								unk1130;				// 1130 - sink offset 198
		EventDispatcher<void>								unk1188;				// 1188 - sink offset 200
		EventDispatcher<void>								unk11E0;				// 11E0 - sink offset 208
		EventDispatcher<void>								unk1238;				// 1238 - sink offset 210
		EventDispatcher<TESUniqueIDChangeEvent>				uniqueIdChangeDispatcher;	// 1290 - sink offset 218
	};

	typedef EventDispatcherList*(*_GetEventDispatcherList)();
	// 614D2151B14D0D61E7DA7088CAB85DB111E2E0D3+1B
	RelocAddr<_GetEventDispatcherList> GetEventDispatcherList(0x00186790);
}