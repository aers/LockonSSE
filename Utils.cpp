#include "../skse64/GameCamera.h"
#include "../skse64/GameData.h"
#include "../skse64/GameReferences.h"
#include "../skse64/GameRTTI.h"

#include "../skse64/NiNodes.h"

#include "TES/GameCamera.h"
#include "Address.h"
#include "Config.h"
#include "Utils.h"

#include <cinttypes>

#define M_PI   3.14159265358979323846264338327950288

void GetAngle(const NiPoint3 &from, const NiPoint3 &to, AngleZX* angle)
{
	const auto x = to.x - from.x;
	const auto y = to.y - from.y;
	const auto z = to.z - from.z;
	const auto xy = sqrt(x*x + y * y);

	angle->z = atan2(x, y);
	angle->x = atan2(-z, xy);
	angle->distance = sqrt(xy*xy + z * z);
}


bool GetAngle(TESObjectREFR* target, AngleZX* angle)
{
	NiPoint3 targetPos;
	NiPoint3 cameraPos;

	if (!GetTargetPos(target, &targetPos))
		return false;
	GetCameraPos(&cameraPos);

	GetAngle(cameraPos, targetPos, angle);

	return true;
}


double NormalAbsoluteAngle(double angle)
{
	while (angle < 0)
		angle += 2 * M_PI;
	while (angle > 2 * M_PI)
		angle -= 2 * M_PI;
	return angle;
}

double NormalRelativeAngle(double angle)
{
	while (angle > M_PI)
		angle -= 2 * M_PI;
	while (angle < -M_PI)
		angle += 2 * M_PI;
	return angle;
}

TESQuest* GetLockOnQuest()
{
	static uint32_t formID = 0;
	TESQuest* quest = nullptr;

	if (formID == 0)
	{
		DataHandler* dhdl = DataHandler::GetSingleton();
		const uint32_t idx = dhdl->GetLoadedModIndex(Config::pluginName.c_str());
		if (idx != 0xFF)
		{
			formID = (idx << 24) | Config::lockonQuestFormID;
		}
	}

	if (formID)
	{
		quest = dynamic_cast<TESQuest*>(LookupFormByID(formID));
	}

	return quest;
}



BGSListForm* GetQuestList()
{
	static uint32_t formIDQuestList = 0;
	BGSListForm* formList = nullptr;

	if (formIDQuestList == 0)
	{
		DataHandler* dhdl = DataHandler::GetSingleton();
		const uint32_t idx = dhdl->GetLoadedModIndex(Config::pluginName.c_str());
		if (idx != 0xFF)
		{
			formIDQuestList = (idx << 24) | Config::questListFormID;
		}
	}

	if (formIDQuestList)
	{
		formList = dynamic_cast<BGSListForm*>(LookupFormByID(formIDQuestList));
	}

	return formList;
}



const char* GetActorName(Actor* akActor)
{
	_DMESSAGE("GetActorName(Actor ptr 0x%016" PRIXPTR ")", akActor);

	static const char unkName[] = "unknown";
	const char* result = unkName;

	if (akActor && akActor->formType == kFormType_Character)
	{
		_DMESSAGE("Actor exists and is a character");

		const auto actorBase = dynamic_cast<TESActorBase*>(akActor->baseForm);
		if (actorBase)
		{
			auto pFullName = dynamic_cast<TESFullName*>(actorBase);
			if (pFullName)
				result = pFullName->GetName();
		}
	}

	return result;
}


void ComputeAnglesFromMatrix(NiMatrix33* mat, NiPoint3* angle)
{
	const double threshold = 0.001;
	if (abs(mat->data[2][1] - 1.0) < threshold)
	{
		angle->x = M_PI / 2;
		angle->y = 0;
		angle->z = atan2(mat->data[1][0], mat->data[0][0]);
	}
	else if (abs(mat->data[2][1] + 1.0) < threshold)
	{
		angle->x = -M_PI / 2;
		angle->y = 0;
		angle->z = atan2(mat->data[1][0], mat->data[0][0]);
	}
	else
	{
		angle->x = asin(mat->data[2][1]);
		angle->y = atan2(-mat->data[2][0], mat->data[2][2]);
		angle->z = atan2(-mat->data[0][1], mat->data[1][1]);
	}
}

bool IsCameraFirstPerson()
{
	PlayerCamera* camera = PlayerCamera::GetSingleton();
	if (!camera)
		return false;

	return camera->cameraState == camera->cameraStates[camera->kCameraState_FirstPerson];
}


bool IsCameraThirdPerson()
{
	PlayerCamera* camera = PlayerCamera::GetSingleton();
	if (!camera)
		return false;

	return camera->cameraState == camera->cameraStates[camera->kCameraState_ThirdPerson2];
}

void GetCameraPos(NiPoint3* pos)
{
	PlayerCharacter* player = *g_thePlayer;
	PlayerCamera* camera = PlayerCamera::GetSingleton();
	float x = 0, y = 0, z = 0;

	if (IsCameraFirstPerson() || IsCameraThirdPerson())
	{
		NiNode* node = camera->cameraNode;
		if (node)
		{
			x = node->m_worldTransform.pos.x;
			y = node->m_worldTransform.pos.y;
			z = node->m_worldTransform.pos.z;
		}
	}
	else
	{
		NiPoint3 playerPos;

		player->GetMarkerPosition(&playerPos);
		z = playerPos.z;
		x = player->pos.x;
		y = player->pos.y;
	}

	pos->x = x;
	pos->y = y;
	pos->z = z;
}


void GetCameraAngle(NiPoint3* pos)
{
	PlayerCamera* camera = PlayerCamera::GetSingleton();
	PlayerCharacter* player = *g_thePlayer;
	float x, y, z;

	if (IsCameraFirstPerson())
	{
		auto fps = dynamic_cast<TES::FirstPersonState*>(camera->cameraState);
		NiPoint3 angle;
		ComputeAnglesFromMatrix(&fps->cameraNode->m_worldTransform.rot, &angle);
		z = NormalAbsoluteAngle(-angle.z);
		x = player->rot.x - angle.x;
		y = angle.y;
	}
	else if (IsCameraThirdPerson())
	{
		const auto tps = dynamic_cast<TES::ThirdPersonState*>(camera->cameraState);
		z = player->rot.z + tps->diffRotZ;
		x = player->rot.x + tps->diffRotX;
		y = 0;
	}
	else
	{
		x = player->rot.x;
		y = player->rot.y;
		z = player->rot.z;
	}

	pos->x = x;
	pos->y = y;
	pos->z = z;
}

// get world coordinates of nodeName for actor
static bool GetNodePosition(Actor* actor, const char* nodeName, NiPoint3* point)
{
	bool bResult = false;

	if (nodeName[0])
	{
		NiAVObject* object = actor->GetNiNode();
		if (object)
		{
			object = object->GetObjectByName(&nodeName);
			if (object)
			{
				point->x = object->m_worldTransform.pos.x;
				point->y = object->m_worldTransform.pos.y;
				point->z = object->m_worldTransform.pos.z;
				bResult = true;
			}
		}
	}

	return bResult;
}



// acquire actor's torso position
static bool GetTorsoPos(Actor* actor, NiPoint3* point)
{
	TESRace* race = actor->race;
	BGSBodyPartData* bodyPart = race->bodyPartData;

	// bodyPart->part[0] body
	// bodyPart->part[1] head
	BGSBodyPartData::Data * data = bodyPart->part[0];
	if (data)
	{
		return GetNodePosition(actor, data->unk08.data, point);
	}

	return false;
}


// get coordinates of target
bool GetTargetPos(TESObjectREFR* target, NiPoint3* pos)
{
	if (target->GetNiNode() == nullptr)
		return false;

	if (target->formType == kFormType_Character)
	{
		if (!GetTorsoPos(dynamic_cast<Actor*>(target), pos))
			target->GetMarkerPosition(pos);
	}
	else
	{
		pos->x = target->pos.x;
		pos->y = target->pos.y;
		pos->z = target->pos.z;
	}

	return true;
}


Actor* GetCombatTarget(Actor* actor)
{
	TESObjectREFR * ref = nullptr;
	UInt32 handle;

	if (actor->IsInCombat())
	{
		handle = actor->unk0FC; // combatTargetRefHandle

		if (handle == *g_invalidRefHandle || handle == 0)
			return nullptr;

		LookupREFRByHandle(&handle, &ref);
	}

	return DYNAMIC_CAST(ref, TESObjectREFR, Actor);
}


bool IsPlayerTeammate(Actor* actor)
{
	return (actor->flags1 & actor->kFlags_IsPlayerTeammate) != 0;
}


bool IsPlayerFollower(Actor* actor)
{
	return Actor_IsInFaction(actor, dynamic_cast<TESFaction*>(LookupFormByID(0x05C84E))); // Follower Faction
}