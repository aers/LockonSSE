#include "../skse64/PapyrusNativeFunctions.h"

#include <algorithm>

#include "lib/EventHelpers.h"
#include "lib/EventFunctors.h"
#include "TES/GameCamera.h"
#include "Address.h"
#include "Events.h"
#include "Hooks.h"
#include "Utils.h"

bool Papyrus_IsGamepadEnabled(StaticFunctionTag * base)
{
	return InputEventDispatcher::GetSingleton() && InputEventDispatcher::GetSingleton()->IsGamepadEnabled();
}

void SetPlayerAngle(float rotZ, float rotX, float wait)
{
	if (wait < 20)
		wait = 20;

	g_TESCameraController->Rotate((*g_thePlayer)->rot.z, (*g_thePlayer)->rot.x, rotZ, rotX, wait, 0);
}

void Papyrus_SetPlayerAngle(StaticFunctionTag * base, float rotZ, float rotX, float wait)
{
	SetPlayerAngle(rotZ, rotX, wait);
}

void LookAt(float posX, float posY, float posZ, float wait)
{
	PlayerCharacter* player = *g_thePlayer;
	NiPoint3 cameraPos;

	GetCameraPos(&cameraPos);

	const double x = posX - cameraPos.x;
	const double y = posY - cameraPos.y;
	const double z = posZ - cameraPos.z;

	const double xy = sqrt(x*x + y * y);
	double rotZ = atan2(x, y);
	const double rotX = atan2(-z, xy);

	if (rotZ - player->rot.z > M_PI)
		rotZ -= M_PI * 2;
	else if (rotZ - player->rot.z < -M_PI)
		rotZ += M_PI * 2;

	SetPlayerAngle(rotZ, rotX, wait);
}

void Papyrus_LookAt(StaticFunctionTag * base, float posX, float posY, float posZ, float wait)
{
	LookAt(posX, posY, posZ, wait);
}

void Papyrus_LookAtRef(StaticFunctionTag * base, TESObjectREFR* akRef, float wait)
{
	if (akRef == nullptr)
		return;

	if (akRef->GetNiNode() == nullptr)
		return;

	NiPoint3 targetPos;
	if (!GetTargetPos(akRef, &targetPos))
		return;

	LookAt(targetPos.x, targetPos.y, targetPos.z, wait);
}

TESObjectREFR* Papyrus_GetCrosshairReference(StaticFunctionTag * base)
{
	CrosshairRefHandleHolder* crh = CrosshairRefHandleHolder::GetSingleton();
	UInt32 handle = crh->CrosshairRefHandle();
	TESObjectREFR* akRef = nullptr;

	if (handle != *g_invalidRefHandle)
	{
		LookupREFRByHandle(&handle, &akRef);
	}

	return akRef;
}

VMResultArray<Actor*> Papyrus_FindCloseActor(TESQuest * thisPtr, float distance, UInt32 sortOrder)
{
	enum Order {
		kSortOrder_distance = 0,		// descending order of distance from actor
		kSortOrder_crosshair = 1,		// descending order of distance from crosshair
		kSortOrder_zaxis_clock = 2,		// z axis clockwise
		kSortOrder_zaxis_rclock = 3,	// z axis counterclockwise
		kSortOrder_invalid = 4
	};

	const double fovThreshold = static_cast<double>(PlayerCamera::GetSingleton()->worldFOV) / 180.0 * M_PI / 2;

	VMResultArray<Actor*> result;

	tArray<UInt32>* actorHandles = &(*g_TESUnkCellInfo)->actorHandles;

	if (actorHandles->count == 0)
		return result;

	std::vector<std::pair<double, Actor*>> vec;
	vec.reserve(actorHandles->count);

	NiPoint3 camPos;
	GetCameraPos(&camPos);

	UInt32 handle;
	size_t i = 0;
	while (actorHandles->GetNthItem(i++, handle))
	{
		TESObjectREFR* ref = nullptr;
		if (handle != *g_invalidRefHandle)
			LookupREFRByHandle(&handle, &ref);

		if (ref && ref->formType == kFormType_Character)
		{
			auto actor = DYNAMIC_CAST(ref, TESObjectREFR, Actor);
			NiPoint3 pos;
			GetTargetPos(actor, &pos);
			const double dx = pos.x - camPos.x;
			const double dy = pos.y - camPos.y;
			const double dz = pos.z - camPos.z;
			const double dd = sqrt(dx*dx + dy * dy + dz * dz);

			if (distance <= 0 || dd <= distance)
			{
				double point;
				NiPoint3 cameraAngle;
				GetCameraAngle(&cameraAngle);
				const double angleZ = NormalRelativeAngle(atan2(dx, dy) - cameraAngle.z);
				const double angleX = NormalRelativeAngle(atan2(-dz, sqrt(dx*dx + dy * dy)) - cameraAngle.x);

				if (abs(angleZ) < fovThreshold)
				{
					switch (sortOrder)
					{
					case kSortOrder_distance:
						point = dd;
						break;
					case kSortOrder_crosshair:
						point = sqrt(angleZ*angleZ + angleX * angleX);
						break;
					case kSortOrder_zaxis_clock:
						point = NormalAbsoluteAngle(atan2(dx, dy) - cameraAngle.z);
						break;
					case kSortOrder_zaxis_rclock:
						point = 2 * M_PI - NormalAbsoluteAngle(atan2(dx, dy) - cameraAngle.z);
						break;
					default:
						point = 0;
						break;
					}

					if (point >= 0)
					{
						vec.emplace_back(point, actor);
					}
				}
			}
		}
	}

	if (vec.empty())
		return result;

	if (sortOrder < kSortOrder_invalid)
		std::sort(vec.begin(), vec.end());

	for (i = 0; i < vec.size(); i++)
	{
		result.push_back(vec[i].second);
	}

	return result;
}

void SendLockonEvent(Actor * actor, const char * eventName)
{
	BGSListForm * list = GetQuestList();

	if (list && list->forms.count)
	{
		for (UInt32 i = 0; i < list->forms.count; i++)
		{
			if (list->forms[i] && list->forms[i]->formType == kFormType_Quest)
			{
				const auto quest = DYNAMIC_CAST(list->forms[i], TESForm, TESQuest);
				const uint64_t handle = EventLib::GetVMHandleForQuest(quest);
				if (handle)
				{
					BSFixedString evStr(eventName);
					EventLib::EventFunctor1<Actor *>(evStr, actor)(handle);
				}
			}
		}
	}
}

void Papyrus_SendLockonStartEvent(TESQuest * thisPtr)
{
	if (thisPtr && EventLib::TESQuest_IsRunning(thisPtr))
	{
		UInt32 handle = *g_invalidRefHandle;
		TESObjectREFR* refTarget = nullptr;

		TESQuest_CreateRefHandleByAliasID(thisPtr, &handle, 0);

		if (handle != *g_invalidRefHandle)
			LookupREFRByHandle(&handle, &refTarget);

		if (refTarget && refTarget->formType == kFormType_Character)
		{
			SendLockonEvent(DYNAMIC_CAST(refTarget, TESObjectREFR, Actor), "OnLock");
		}
	}
}

void Papyrus_SendLockonStopEvent(TESQuest * thisPtr)
{
	if (thisPtr && EventLib::TESQuest_IsRunning(thisPtr))
	{
		UInt32 handle = *g_invalidRefHandle;
		TESObjectREFR* refTarget = nullptr;

		TESQuest_CreateRefHandleByAliasID(thisPtr, &handle, 0);

		if (handle != *g_invalidRefHandle)
			LookupREFRByHandle(&handle, &refTarget);

		if (refTarget && refTarget->formType == kFormType_Character)
		{
			SendLockonEvent(DYNAMIC_CAST(refTarget, TESObjectREFR, Actor), "OnUnlock");
		}
	}
}

void Papyrus_SetCameraSpeed(StaticFunctionTag * base, float speed)
{
	g_cameraSpeed = speed;
}

float Papyrus_GetThumbstickAxisX(StaticFunctionTag * base, UInt32 type)
{
	if (type == 0x0B)
		return g_leftThumbstick.x;
	else if (type == 0x0C)
		return g_rightThumbstick.x;

	return 0;
}

float Papyrus_GetThumbstickAxisY(StaticFunctionTag * base, UInt32 type)
{
	if (type == 0x0B)
		return g_leftThumbstick.y;
	else if (type == 0x0C)
		return g_rightThumbstick.y;

	return 0;
}

void Papyrus_ResetMouse(StaticFunctionTag * base)
{
	g_mousePosition.x = 0;
	g_mousePosition.y = 0;
}

SInt32 Papyrus_GetMouseX(StaticFunctionTag * base)
{
	return g_mousePosition.x;
}

SInt32 Papyrus_GetMouseY(StaticFunctionTag * base)
{
	return g_mousePosition.y;
}

namespace Papyrus
{
	bool Init(VMClassRegistry* registry)
	{
		registry->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, bool>("IsGamepadEnabled", "LockOn_Main", Papyrus_IsGamepadEnabled, registry));
		registry->SetFunctionFlags("LockOn_Main", "IsGamepadEnabled", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(
			new NativeFunction3<StaticFunctionTag, void, float, float, float>("SetPlayerAngle", "LockOn_Main", Papyrus_SetPlayerAngle, registry));

		registry->RegisterFunction(
			new NativeFunction4<StaticFunctionTag, void, float, float, float, float>("LookAt", "LockOn_Main", Papyrus_LookAt, registry));

		registry->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, TESObjectREFR*, float>("LookAtRef", "LockOn_Main", Papyrus_LookAtRef, registry));

		registry->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, TESObjectREFR*>("GetCrosshairReference", "LockOn_Main", Papyrus_GetCrosshairReference, registry));

		registry->RegisterFunction(
			new NativeFunction2<TESQuest, VMResultArray<Actor *>, float, UInt32>("FindCloseActor", "LockOn_Main", Papyrus_FindCloseActor, registry));

		registry->RegisterFunction(
			new NativeFunction0<TESQuest, void>("SendLockonStartEvent", "LockOn_Main", Papyrus_SendLockonStartEvent, registry));

		registry->RegisterFunction(
			new NativeFunction0<TESQuest, void>("SendLockonStopEvent", "LockOn_Main", Papyrus_SendLockonStopEvent, registry));

		registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, float>("SetCameraSpeed", "LockOn_Main", Papyrus_SetCameraSpeed, registry));
		registry->SetFunctionFlags("LockOn_Main", "SetCameraSpeed", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, float, UInt32>("GetThumbstickAxisX", "LockOn_Main", Papyrus_GetThumbstickAxisX, registry));
		registry->SetFunctionFlags("LockOn_Main", "GetThumbstickAxisX", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, float, UInt32>("GetThumbstickAxisY", "LockOn_Main", Papyrus_GetThumbstickAxisY, registry));
		registry->SetFunctionFlags("LockOn_Main", "SetThumbstickAxisY", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, void>("ResetMouse", "LockOn_Main", Papyrus_ResetMouse, registry));
		registry->SetFunctionFlags("LockOn_Main", "ResetMouse", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, SInt32>("GetMouseX", "LockOn_Main", Papyrus_GetMouseX, registry));
		registry->SetFunctionFlags("LockOn_Main", "GetMouseX", VMClassRegistry::kFunctionFlag_NoWait);

		registry->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, SInt32>("GetMouseY", "LockOn_Main", Papyrus_GetMouseY, registry));
		registry->SetFunctionFlags("LockOn_Main", "GetMouseY", VMClassRegistry::kFunctionFlag_NoWait);

		return true;
	}
}