#pragma once

#include "../../skse64/PapyrusVM.h"
#include "../../skse64/GameForms.h"
#include "../../skse64/GameRTTI.h"

#include <cinttypes>

namespace EventLib
{
	// original function: 0x009B49C0
	// unk148 is 0x248 see skse64 TESQuest, left over naming from skse probs
	inline bool TESQuest_IsRunning(TESQuest* quest)
	{
		return (quest->unk0D8.flags & 1) != 0 && (quest->unk0D8.flags >> 7 & 1) == 0 && quest->unk148 == 0;
	}

	inline UInt64 GetVMHandleForQuest(TESQuest* quest, uint64_t aliasIndex = 0)
	{
		_DMESSAGE("[DEBUG] GetVMHandleForQuest(TESQuest ptr 0x%016" PRIXPTR ")", quest);

		VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();

		UInt64 handle = 0;

		if (registry && quest && TESQuest_IsRunning(quest))
		{
			_DMESSAGE("[DEBUG] Quest exists and is running");

			BGSBaseAlias* baseAlias = nullptr;
			if (quest->aliases.GetNthItem(aliasIndex, baseAlias))
			{
				const auto refAlias = DYNAMIC_CAST(baseAlias, BGSBaseAlias, BGSRefAlias);
				if (refAlias)
				{
					IObjectHandlePolicy* policy = registry->GetHandlePolicy();
					handle = policy->Create(refAlias->kTypeID, refAlias);

					_DMESSAGE("[DEBUG] Quest handle is %d", handle);
				}
			}
		}

		return handle;
	}
}