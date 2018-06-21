#include "../skse64/GameEvents.h"
#include "../skse64/GameInput.h"
#include "../skse64/GameReferences.h"

#include "TES/InputEvents.h"
#include "lib/EventFunctors.h"
#include "lib/EventHelpers.h"
#include "Events.h"
#include "Hooks.h"
#include "Utils.h"
#include "TES/EventDispatcherList.h"

ThumbstickInfo g_leftThumbstick = { 0, 0 };
ThumbstickInfo g_rightThumbstick = { 0, 0 };
MouseInfo g_mousePosition = { 0, 0 };

class LockOn_CombatEventHandler : public BSTEventSink<TESCombatEvent>
{
public:
	EventResult ReceiveEvent(TESCombatEvent* evn, EventDispatcher<TESCombatEvent>* dispatcher) override
	{
		if (evn->state != 1)
			return kEvent_Continue;

		if (evn->target == nullptr || evn->source == nullptr)
			return kEvent_Continue;

		const auto target = DYNAMIC_CAST(evn->target, TESObjectREFR, Actor);
		const auto source = DYNAMIC_CAST(evn->source, TESObjectREFR, Actor);

#ifdef _DEBUG
		const char* casterName = GetActorName(source);
		const char* targetName = GetActorName(target);

		switch (evn->state)
		{
		case 0:
			_DMESSAGE("[DEBUG] Not in combat:\n  caster: %s", casterName);
			break;
		case 1:
			_DMESSAGE("[DEBUG] In combat:\n  caster: %s\n  target: %s", casterName, targetName);
			break;
		case 2:
			_DMESSAGE("[DEBUG] Searching:\n  caster: %s\n  target: %s", casterName, targetName);
			break;
		default:
			break;
		}
#endif

		const auto player = *g_thePlayer;

		if (source == player)
			return kEvent_Continue;

		Actor * enemy = nullptr;

		if (target == player || IsPlayerTeammate(target))
			enemy = source;
		else if (IsPlayerTeammate(source))
			enemy = target;

		if (enemy)
		{
			_DMESSAGE("[DEBUG] enemy");

			auto * quest = GetLockOnQuest();
			if (quest && EventLib::TESQuest_IsRunning(quest))
			{
				const uint64_t handle = EventLib::GetVMHandleForQuest(quest, 1);
				if (handle)
				{
					static BSFixedString eventName("Lockon_OnCombatStart");
					EventLib::EventFunctor1<Actor *>(eventName, enemy)(handle);
				}
			}
		}

		return kEvent_Continue;
	}
};


class LockOn_HitEventHandler : public BSTEventSink<TESHitEvent>
{
public:
	EventResult ReceiveEvent(TESHitEvent * evn, EventDispatcher<TESHitEvent> * dispatcher) override
	{
		if (!evn->caster || evn->caster != *g_thePlayer)
			return kEvent_Continue;

		TESQuest* quest = GetLockOnQuest();

		if (quest && EventLib::TESQuest_IsRunning(quest))
		{
			TESForm* akSource = nullptr;
			Projectile* akProjectile = nullptr;

			if (evn->sourceFormID != 0)
			{
				akSource = LookupFormByID(evn->sourceFormID);
				if (akSource != nullptr) {
					if (akSource->formType != kFormType_Weapon
						&& akSource->formType != kFormType_Explosion
						&& akSource->formType != kFormType_Spell
						&& akSource->formType != kFormType_Ingredient
						&& akSource->formType != kFormType_Potion
						&& akSource->formType != kFormType_Enchantment)
					{
						akSource = nullptr;
					}
				}
			}

			if (evn->projectileFormID != 0)
			{
				akProjectile = dynamic_cast<Projectile*>(LookupFormByID(evn->projectileFormID));
				if (akProjectile != nullptr && akProjectile->formType != kFormType_Projectile)
				{
					akProjectile = nullptr;
				}
			}

			const uint64_t handle = EventLib::GetVMHandleForQuest(quest, 1);
			if (handle)
			{
				static BSFixedString eventName("Lockon_OnPlayerHit");
				EventLib::EventFunctor3<TESObjectREFR *, TESForm *, Projectile *>(eventName, evn->target, akSource, akProjectile)(handle);
			}
		}

		return kEvent_Continue;
	}
};

static void OnThumbstickEvent(TES::ThumbstickEvent * evt, TESQuest * quest)
{
	static bool bThumbstickLeft = false;
	static bool bThumbstickRight = false;

	bool  bTrigger = false;
	const bool bState = (evt->x != 0 || evt->y != 0);

	if (evt->keyMask == evt->kInputType_LeftThumbstick)
	{
		g_leftThumbstick.x = evt->x;
		g_leftThumbstick.y = evt->y;

		if (bThumbstickLeft != bState)
		{
			bThumbstickLeft = bState;
			bTrigger = true;
		}
	}
	else if (evt->keyMask == evt->kInputType_RightThumbstick)
	{
		g_rightThumbstick.x = evt->x;
		g_rightThumbstick.y = evt->y;

		if (bThumbstickRight != bState)
		{
			bThumbstickRight = bState;
			bTrigger = true;
		}
	}

	if (bTrigger)
	{
		const uint64_t handle = EventLib::GetVMHandleForQuest(quest);
		if (handle)
		{
			static BSFixedString eventName = "Lockon_OnThumbstick";
			EventLib::EventFunctor3<uint32_t, float, float>(eventName, evt->keyMask, evt->x, evt->y)(handle);
		}
	}
}

static void OnMouseMoveEvent(TES::MouseMoveEvent * evt, TESQuest * quest)
{
	static bool   bMoving = false;
	static SInt32 totalX = 0;
	static SInt32 totalY = 0;

	if (evt->source != evt->kInputType_Mouse)
		return;

	bool  bTrigger = false;
	const bool  bState = (evt->moveX != 0 || evt->moveY != 0);
	_DMESSAGE("MouseMove: %08X %08X %08X", evt->source, evt->moveX, evt->moveY);

	totalX += evt->moveX;
	totalY += evt->moveY;
	g_mousePosition.x += evt->moveX;
	g_mousePosition.y += evt->moveY;

	if (g_mousePosition.x > MOUSE_THRESHOLD)
		g_mousePosition.x = MOUSE_THRESHOLD;
	if (g_mousePosition.x < -MOUSE_THRESHOLD)
		g_mousePosition.x = -MOUSE_THRESHOLD;
	if (g_mousePosition.y > MOUSE_THRESHOLD)
		g_mousePosition.y = MOUSE_THRESHOLD;
	if (g_mousePosition.y < -MOUSE_THRESHOLD)
		g_mousePosition.y = -MOUSE_THRESHOLD;

	if (bMoving != bState)
	{
		bMoving = bState;
		bTrigger = true;
	}

	if (bTrigger && !bState)
	{
		const uint64_t handle = EventLib::GetVMHandleForQuest(quest);
		if (handle)
		{
			static BSFixedString eventName("Lockon_OnMouse");
			EventLib::EventFunctor2<int32_t, int32_t>(eventName, totalX, totalY)(handle);
		}
	}
}

class Lockon_InputEventHandler : public BSTEventSink<InputEvent>
{
public:
	EventResult ReceiveEvent(InputEvent ** evns, InputEventDispatcher * dispatcher) override
	{
		auto * quest = GetLockOnQuest();

		if (!quest || !EventLib::TESQuest_IsRunning(quest))
			return kEvent_Continue;

		for (InputEvent * e = *evns; e; e = e->next)
		{
			if (e->eventType == InputEvent::kEventType_Thumbstick)
			{
				const auto t = dynamic_cast<TES::ThumbstickEvent*>(e);
				OnThumbstickEvent(t, quest);
			}
			else if (e->eventType == InputEvent::kEventType_MouseMove)
			{
				const auto m = dynamic_cast<TES::MouseMoveEvent*>(e);
				OnMouseMoveEvent(m, quest);
			}
		}

		return kEvent_Continue;
	}
};

LockOn_HitEventHandler g_hitEventHandler;
Lockon_InputEventHandler g_inputEventHandler;

namespace Events
{
	void Init()
	{
		auto * inputEventDispatcher = InputEventDispatcher::GetSingleton();
		if (inputEventDispatcher)
		{
			inputEventDispatcher->AddEventSink(&g_inputEventHandler);
		}
		const auto eventDispatcherList = TES::GetEventDispatcherList();
		auto hitEventDispatcher = eventDispatcherList->hitDispatcher;
		hitEventDispatcher.AddEventSink(&g_hitEventHandler);
	}
}