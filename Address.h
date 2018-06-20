#pragma once

#include "../skse64_common/Relocation.h"

#include "../skse64/GameReferences.h"

// Skyrim SE 1.5.39

// Utils.cpp

// ??_7Actor@@6B@ + 0x7C8 (vtbl F9)
typedef bool(*_Actor_IsInFaction)(Actor * thisPtr, TESFaction * faction);
RelocAddr<_Actor_IsInFaction> Actor_IsInFaction(0x005F89B0);