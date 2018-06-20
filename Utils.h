#pragma once

#define DEFINE_MEMBER_FN_EX(className, functionName, retnType, address, ...)		\
	typedef retnType (className::* _##functionName##_type)(__VA_ARGS__);			\
																					\
	inline _##functionName##_type * _##functionName##_GetPtr(void)					\
	{																				\
		static const uintptr_t _address = address;										\
		return (_##functionName##_type *)&_address;									\
	}

#define CALL_MEMBER_FN_EX(obj, fn)	\
	((*(obj)).*(*(_##fn##_GetPtr())))


struct AngleZX
{
	double  z;
	double  x;
	double  distance;
};


class TESObjectREFR;
class TESQuest;
class BGSListForm;
class Actor;
class NiPoint3;
class NiMatrix33;

void ComputeAnglesFromMatrix(NiMatrix33* mat, NiPoint3* angle);
void GetAngle(const NiPoint3 &from, const NiPoint3 &to, AngleZX* angle);
bool GetAngle(TESObjectREFR* target, AngleZX* angle);
TESQuest* GetLockOnQuest(void);
BGSListForm* GetQuestList(void);
const char* GetActorName(Actor* akActor);
bool IsCameraFirstPerson();
bool IsCameraThirdPerson();
void GetCameraPos(NiPoint3* pos);
void GetCameraAngle(NiPoint3* angle);
bool GetTargetPos(TESObjectREFR* target, NiPoint3* pos);
Actor* GetCombatTarget(Actor* actor);
bool IsPlayerTeammate(Actor* actor);
bool IsPlayerFollower(Actor* actor);

double NormalAbsoluteAngle(double angle);
double NormalRelativeAngle(double angle);

inline bool TESQuest_IsRunning(TESQuest* quest);
