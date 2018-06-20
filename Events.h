#pragma once

#define MOUSE_THRESHOLD 320

struct ThumbstickInfo
{
	float x;
	float y;
};

struct MouseInfo
{
	SInt32 x;
	SInt32 y;
};


extern ThumbstickInfo g_leftThumbstick;
extern ThumbstickInfo g_rightThumbstick;
extern MouseInfo g_mousePosition;


namespace Events
{
	void Init(void);
}