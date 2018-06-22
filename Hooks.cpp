
#include "TES/GameCamera.h"
#include "Address.h"
#include "Utils.h"
#include "Events.h"
#include "Hooks.h"

float g_cameraSpeed = 500.0;
double g_targetPosX = 0.0f;
double g_targetPosY = 0.0f;
double g_targetDist = 0.0f;
UInt32 g_targetFormID = 0;
const char* g_targetName = nullptr;

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

	PlayerCamera* camera = PlayerCamera::GetSingleton();
	PlayerCharacter* player = *g_thePlayer;

	double angleZ = NormalAbsoluteAngle((double)player->rot.z + angle->z / (g_cameraSpeed * 60 / 2000));
	double angleX = NormalRelativeAngle((double)player->rot.x + angle->x / (g_cameraSpeed * 60 / 2000));

	if ((player->actorState.flags04 & 0x0003C000) == 0)
	{
		// 納刀時？
		if (IsCameraThirdPerson())
		{
			const auto tps = dynamic_cast<TES::ThirdPersonState*>(camera->cameraState);
			tps->diffRotZ = 0.0;
			tps->diffRotX = 0.0;
		}
		CALL_MEMBER_FN_EX(player, SetAngleZ)(angleZ);
		CALL_MEMBER_FN_EX(player, SetAngleX)(angleX);
	}
	else
	{
		// 抜刀時？
		if (IsCameraFirstPerson())
		{
			auto* fps = dynamic_cast<TES::FirstPersonState*>(camera->cameraState);
			angleZ -= player->Unk_A3(0);

			fps->unk20 = angleZ;
			CALL_MEMBER_FN_EX(player, SetAngleX)(angleX);
		}
		else if (IsCameraThirdPerson())
		{
			ThirdPersonState* tps = (ThirdPersonState*)camera->cameraState;
			angleZ -= camera->unkC4;

			tps->diffRotZ = angleZ;
			tps->diffRotX = 0;
		}
		else
		{
			CALL_MEMBER_FN_EX(player, SetAngleZ)(angleZ);
			CALL_MEMBER_FN_EX(player, SetAngleX)(angleX);
		}
	}
}