#include "Address.h"

// Skyrim SE 1.5.39

// ??_7Actor@@6B@ + 0x7C8 (vtbl F9)
RelocAddr<_Actor_IsInFaction> Actor_IsInFaction(0x005F89B0);

RelocPtr<TES::TESCameraController> g_TESCameraController(0x01E16A10);

RelocPtr<TES::UnkCellInfo *> g_TESUnkCellInfo(0x01EE5AD0);

// E8 ? ? ? ? EB 48 33 D2 ->
RelocAddr<_TESQuest_CreateRefHandleByAliasID> TESQuest_CreateRefHandleByAliasID(0x003789C0);

// ini setting bInvertYValues:Controls
RelocPtr<bool> ini_bInvertYValues(0x01E17770);