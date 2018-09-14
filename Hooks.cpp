#include "../skse64_common/BranchTrampoline.h"

#include "../skse64/GameMenus.h"

#include "TES/GameCamera.h"
#include "Address.h"
#include "Utils.h"
#include "Events.h"
#include "Hooks.h"
#include "lib/EventHelpers.h"

float g_cameraSpeed = 500.0;
double g_targetPosX = 0.0f;
double g_targetPosY = 0.0f;
double g_targetDist = 0.0f;
UInt32 g_targetFormID = 0;
const char* g_targetName = nullptr;

template <class T>
struct _SharedPtrInternal
{
public:
	static void IncRef(T* p);
	static void DecRef(T* p);
};


template <>
struct _SharedPtrInternal<TESObjectREFR>
{
	static void IncRef(TESObjectREFR* p) { p->handleRefObject.IncRef(); }
	static void DecRef(TESObjectREFR* p) { p->handleRefObject.DecRefHandle(); }
};


template <class T, class I = _SharedPtrInternal<T>>
class SharedPtr
{
public:
	SharedPtr() : pointer(NULL) {}
	~SharedPtr()
	{
		if (pointer)
			I::DecRef(pointer);
	}

	operator T*() { return pointer; }
	T* operator->() { return pointer; }
	bool operator!() { return pointer != NULL; }

	SharedPtr& operator=(T* refr) {
		I::IncRef(refr);
		if (pointer)
			I::DecRef(pointer);
		pointer = refr;
		return *this;
	}

	bool LookupByHandle(UInt32 handle) {
		return LookupSharedPtrByHandle(handle, this);
	}

private:
	T * pointer;
};

#define ADDR_LookupSharedPtrByHandle			0x00132BC0

template<class T>
bool LookupSharedPtrByHandle(UInt32 handle, SharedPtr<T>* mp)
{
	bool(*Lookup)(UInt32*, SharedPtr<T>*) = (bool(*)(UInt32*, SharedPtr<T>*))(RelocationManager::s_baseAddr + ADDR_LookupSharedPtrByHandle);
	return Lookup(&handle, mp);
}

static void CalcAngle(NiPoint3* targetPos, AngleZX* angle)
{
	PlayerCharacter* player = *g_thePlayer;
	AngleZX baseAngle { 0, 0, 0 };
	NiPoint3 cameraPos;

	GetCameraPos(&cameraPos);
	GetAngle(cameraPos, *targetPos, &baseAngle);

	double angleDiffZ = baseAngle.z - static_cast<double>(player->rot.z);
	double angleDiffX = baseAngle.x - static_cast<double>(player->rot.x);

	// enable angle adjustment with thumbstick
	if (InputEventDispatcher::GetSingleton() && InputEventDispatcher::GetSingleton()->IsGamepadEnabled())
	{
		const double newAngle = atan2(128.0, baseAngle.distance);
		angleDiffZ += static_cast<double>(g_rightThumbstick.x) * newAngle;

		if (*ini_bInvertYValues)
			angleDiffX += static_cast<double>(g_rightThumbstick.y) * newAngle;
		else
			angleDiffX -= static_cast<double>(g_rightThumbstick.y) * newAngle;
	}

	angle->z = NormalRelativeAngle(angleDiffZ);
	angle->x = NormalRelativeAngle(angleDiffX);
	angle->distance = baseAngle.distance;
}

static void RotateCamera(AngleZX* angle)
{
	if ((*g_TESCameraController).unk1C == 0)
		return;

	const auto camera = reinterpret_cast<TES::PlayerCamera *>(PlayerCamera::GetSingleton());
	PlayerCharacter* player = *g_thePlayer;

	double angleZ = NormalAbsoluteAngle(static_cast<double>(player->rot.z) + angle->z / (g_cameraSpeed * 60 / 2000));
	double angleX = NormalRelativeAngle(static_cast<double>(player->rot.x) + angle->x / (g_cameraSpeed * 60 / 2000));

	if ((player->actorState.flags04 & 0x0003C000) == 0)
	{
		// 納刀時？
		if (IsCameraThirdPerson())
		{
			const auto tps = reinterpret_cast<TES::ThirdPersonState*>(camera->cameraState);
			tps->diffRotZ = 0.0;
			tps->diffRotX = 0.0;
		}
		PlayerCharacter_SetAngleZ(player, angleZ);
		PlayerCharacter_SetAngleX(player, angleX);
	}
	else
	{
		// 抜刀時？
		if (IsCameraFirstPerson())
		{
			const auto fps = reinterpret_cast<TES::FirstPersonState*>(camera->cameraState);
			angleZ -= PlayerCharacter_UnkA4(player, 0);

			fps->unk68[0] = angleZ;
			PlayerCharacter_SetAngleX(player, angleX);
		}
		else if (IsCameraThirdPerson())
		{
			const auto tps = reinterpret_cast<TES::ThirdPersonState*>(camera->cameraState);
			angleZ -= camera->unk154;

			tps->diffRotZ = angleZ;
			tps->diffRotX = 0;
		}
		else
		{
			PlayerCharacter_SetAngleZ(player, angleZ);
			PlayerCharacter_SetAngleX(player, angleX);
		}
	}
}

static void OnCameraMove(uintptr_t * unk)
{
	g_targetFormID = 0;

	static BSFixedString menuName("Dialogue Menu");
	MenuManager* mm = MenuManager::GetSingleton();
	if (!mm || mm->IsMenuOpen(&menuName))
		return;

	TESQuest* quest = GetLockOnQuest();
	if (!quest || !EventLib::TESQuest_IsRunning(quest))
		return;

	UInt32 handle = *g_invalidRefHandle;

	TESQuest_CreateRefHandleByAliasID(quest, &handle, 0);

	if (handle == *g_invalidRefHandle)
		return;

	SharedPtr<TESObjectREFR> refTarget;
	if (!refTarget.LookupByHandle(handle))
		return;

	NiPoint3 cameraPos, cameraAngle, targetPos;
	if (GetTargetPos(refTarget, &targetPos))
	{
		AngleZX targetAngle{0, 0, 0};
		const double fov = 1 / tan(PlayerCamera::GetSingleton()->worldFOV * M_PI / 360.0);

		// HUD表示・透視投影変換式
		GetCameraPos(&cameraPos);
		GetCameraAngle(&cameraAngle);
		GetAngle(cameraPos, targetPos, &targetAngle);
		targetAngle.z = NormalRelativeAngle(targetAngle.z - cameraAngle.z);
		targetAngle.x = NormalRelativeAngle(targetAngle.x - cameraAngle.x);
		g_targetFormID = refTarget->formID;
		const double distance = targetAngle.distance * cos(targetAngle.z) * cos(targetAngle.x);
		double x = targetAngle.distance * sin(targetAngle.z);
		double y = targetAngle.distance * sin(targetAngle.x);

		if (abs(cameraAngle.y) > 0)
		{
			const double x2 = x * cos(cameraAngle.y) - y * sin(cameraAngle.y);
			const double y2 = y * cos(cameraAngle.y) + x * sin(cameraAngle.y);
			x = x2;
			y = y2;
		}

		static UInt32 screenWidth = 0;
		static UInt32 screenHeight = 0;
		if (screenHeight == 0)
		{
			screenWidth = GetINISetting("iSize W:Display")->data.u32;
			screenHeight = GetINISetting("iSize H:Display")->data.u32;
		}

		g_targetPosX = x / distance * fov * 800.0 / screenWidth * screenHeight;
		g_targetPosY = y / distance * fov * 480.0;
		g_targetDist = targetAngle.distance;
		const auto actorBase = DYNAMIC_CAST(refTarget->baseForm, TESForm, TESActorBase);
		g_targetName = "";
		if (actorBase)
		{
			const auto fullName = DYNAMIC_CAST(actorBase, TESActorBase, TESFullName);
			if (fullName && fullName->name.data)
			{
				g_targetName = fullName->name.data;
			}
		}

		// カメラ回転
		CalcAngle(&targetPos, &targetAngle);
		RotateCamera(&targetAngle);
	}
}

namespace Hooks
{
	void Init()
	{
		_MESSAGE("Initializing hooks");
		g_branchTrampoline.Write5Call(OnCameraMoveHook.GetUIntPtr(), GetFnAddr(OnCameraMove));
	}
}