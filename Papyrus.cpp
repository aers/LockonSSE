#include "../skse64/PapyrusNativeFunctions.h"

#include <algorithm>

#include "TES/GameCamera.h"
#include "Address.h"
#include "Utils.h"

#define M_PI   3.14159265358979323846264338327950288

bool IsGamepadEnabled(StaticFunctionTag * base)
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

static TESObjectREFR* Papyrus_GetCrosshairReference(StaticFunctionTag * base)
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

VMResultArray<Actor*> FindCloseActor(TESQuest * thisPtr, float distance, UInt32 sortOrder)
{
	enum Order {
		kSortOrder_distance = 0,		// 距離が近い順
		kSortOrder_crosshair = 1,		// クロスヘアに近い順
		kSortOrder_zaxis_clock = 2,		// Z軸時計回り
		kSortOrder_zaxis_rclock = 3,		// Z軸逆時計回り
		kSortOrder_invalid = 4
	};

	double fovThreshold = (double)PlayerCamera::GetSingleton()->worldFOV / 180.0 * M_PI / 2;

	VMResultArray<Actor*> result;

	tArray<UInt32>* actorHandles = &(*g_TESUnkCellInfo)->actorHandles;

	if (actorHandles->count == 0)
		return result;

	std::vector<std::pair<double, Actor*>> vec;
	vec.reserve(actorHandles->count);

	PlayerCharacter* player = *g_thePlayer;
	NiPoint3 camPos;
	GetCameraPos(&camPos);

	UInt32 handle;
	size_t i = 0;
	while (actorHandles->GetNthItem(i++, handle))
	{
		TESObjectREFR* ref = NULL;
		if (handle != *g_invalidRefHandle)
			LookupREFRByHandle(&handle, &ref);

		if (ref && ref->formType == kFormType_Character)
		{
			Actor* actor = (Actor*)ref;
			NiPoint3 pos;
			GetTargetPos(actor, &pos);
			double dx = pos.x - camPos.x;
			double dy = pos.y - camPos.y;
			double dz = pos.z - camPos.z;
			double dd = sqrt(dx*dx + dy * dy + dz * dz);

			if (distance <= 0 || dd <= distance)
			{
				double point;
				NiPoint3 cameraAngle;
				GetCameraAngle(&cameraAngle);
				double angleZ = NormalRelativeAngle(atan2(dx, dy) - cameraAngle.z);
				double angleX = NormalRelativeAngle(atan2(-dz, sqrt(dx*dx + dy * dy)) - cameraAngle.x);

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
						vec.push_back(std::make_pair(point, actor));
					}
				}
			}
		}
	}

	if (vec.empty())
		return result;

	if (sortOrder < kSortOrder_invalid)
		std::sort(vec.begin(), vec.end());

	// Papyrusに返す配列を確保
	for (i = 0; i < vec.size(); i++)
	{
		result.push_back(vec[i].second);
	}

	return result;
}


namespace Papyrus
{
	void Init()
	{
		g_TESCameraController->Rotate(1, 1, 1, 1, 1, 1);
	}
}