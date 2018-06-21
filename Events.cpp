#include "../skse64/GameInput.h"

#include "TES/InputEvents.h"
#include "lib/EventFunctors.h"
#include "lib/EventHelpers.h"
#include "Events.h"
#include "Hooks.h"
#include "Utils.h"

ThumbstickInfo g_leftThumbstick = { 0, 0 };
ThumbstickInfo g_rightThumbstick = { 0, 0 };
MouseInfo g_mousePosition = { 0, 0 };

static void OnThumbstickEvent(TES::ThumbstickEvent * evt, TESQuest * quest)
{
	static bool bThumbstickLeft = false;
	static bool bThumbstickRight = false;

	bool  bTrigger = false;
	bool  bState = (evt->x != 0 || evt->y != 0);

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
	bool  bState = (evt->moveX != 0 || evt->moveY != 0);
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
				auto t = dynamic_cast<TES::ThumbstickEvent*>(e);
				OnThumbstickEvent(t, quest);
			}
			else if (e->eventType == InputEvent::kEventType_MouseMove)
			{
				auto m = dynamic_cast<TES::MouseMoveEvent*>(e);
				OnMouseMoveEvent(m, quest);
			}
		}

		return kEvent_Continue;
	}
};

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
	}
}