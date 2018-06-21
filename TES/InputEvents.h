#pragma once

#include "../../skse64/GameInput.h"

namespace TES
{
	// 30
	class MouseMoveEvent : public IDEvent, public InputEvent
	{
	public:
		virtual					~MouseMoveEvent();
		virtual bool			IsIDEvent();
		virtual BSFixedString *	GetControlID();

		enum { kInputType_Mouse = 0x0A };
								// 0x18, controlID from IDEvent
		uint32_t source;		// 0x20, init 0x0A
		uint32_t pad24;			// 0x24, pad
		int32_t  moveX;			// 0x28
		int32_t  moveY;			// 0x2C
	};

	// 30
	class ThumbstickEvent : public IDEvent, public InputEvent
	{
	public:
		virtual					~ThumbstickEvent();
		virtual bool			IsIDEvent();
		virtual BSFixedString *	GetControlID();

		enum {									// (himika)
			kInputType_LeftThumbstick = 0x0B,	//
			kInputType_RightThumbstick = 0x0C	//
		};										//


		bool IsLeft(void) const {
			return keyMask == kInputType_LeftThumbstick;
		}

		bool IsRight(void) const {
			return keyMask == kInputType_RightThumbstick;
		}

		// 18 -> controlID from IDEvent
		uint32_t keyMask;	// 20 - b for left stick, c for right stick
		uint32_t pad24;     // 24
		float	 x;			// 28
		float	 y;			// 2C
	};
}