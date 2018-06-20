#include "../skse64/GameInput.h"
#include "../skse64/GameRTTI.h"

#include "Events.h"
#include "Hooks.h"
#include "Utils.h"

ThumbstickInfo g_leftThumbstick = { 0, 0 };
ThumbstickInfo g_rightThumbstick = { 0, 0 };
MouseInfo g_mousePosition = { 0, 0 };

static void OnThumbstickEvent(ThumbstickEvent * evt, TESQuest * quest)
{
	return;
}

static void OnMouseMoveEvent(MouseMoveEvent * evt, TESQuest * quest)
{
	static bool   bMoving = false;
	static SInt32 totalX = 0;
	static SInt32 totalY = 0;

	if (evt->source != evt->kInputType_Mouse)
		return;

	bool  bTrigger = false;
	bool  bState = (evt->moveX != 0 || evt->moveY != 0);
	// _MESSAGE("MouseMove: %08X %08X %08X", evt->source, evt->moveX, evt->moveY);

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

	if (bTrigger && bState == false)
	{
		static BSFixedString eventName = "Lockon_OnMouse";
		PapyrusUtil::SendEvent(quest, &eventName, totalX, totalY);
	}
}

class Lockon_InputEventHandler : public BSTEventSink<InputEvent>
{
public:
	EventResult ReceiveEvent(InputEvent ** evns, InputEventDispatcher * dispatcher) override
	{
		auto * quest = GetLockOnQuest();

		if (!quest || !TESQuest_IsRunning(quest))
			return kEvent_Continue;

		for (InputEvent * e = *evns; e; e = e->next)
		{
			if (e->eventType == InputEvent::kEventType_Thumbstick)
			{
				auto t = DYNAMIC_CAST(e, InputEvent, ThumbstickEvent);
				OnThumbstickEvent(t, quest);
			}
			else if (e->eventType == InputEvent::kEventType_MouseMove)
			{
				auto m = DYNAMIC_CAST(e, InputEvent, MouseMoveEvent);
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