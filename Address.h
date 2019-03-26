#pragma once

#include "../skse64_common/Relocation.h"

#include "../skse64/GameForms.h"
#include "../skse64/GameReferences.h"

#include "TES/GameCamera.h"
#include "TES/UnkCellInfo.h"

typedef bool(*_Actor_IsInFaction)(Actor * thisPtr, TESFaction * faction);
extern RelocAddr<_Actor_IsInFaction> Actor_IsInFaction;

extern RelocPtr<TES::TESCameraController> g_TESCameraController;

extern RelocPtr<TES::UnkCellInfo *> g_TESUnkCellInfo;

typedef uint32_t*(*_TESQuest_CreateRefHandleByAliasID)(TESQuest * thisPtr, UInt32 * handle, UInt32 aliasID);
extern RelocAddr<_TESQuest_CreateRefHandleByAliasID> TESQuest_CreateRefHandleByAliasID;

extern RelocPtr<bool> ini_bInvertYValues;

typedef void(*_PlayerCharacter_SetAngleZ)(PlayerCharacter * thisPtr, float angle);
extern RelocAddr<_PlayerCharacter_SetAngleZ> PlayerCharacter_SetAngleZ;

typedef void(*_PlayerCharacter_SetAngleX)(PlayerCharacter * thisPtr, float angle);
extern RelocAddr<_PlayerCharacter_SetAngleX> PlayerCharacter_SetAngleX;

typedef float(*_PlayerCharacter_UnkA4)(PlayerCharacter * thisPtr, UInt32 arg1);
extern RelocAddr<_PlayerCharacter_UnkA4> PlayerCharacter_UnkA4;

extern RelocAddr<uintptr_t> OnCameraMoveHook;

#define ADDR_LookupSharedPtrByHandle			0x001329D0