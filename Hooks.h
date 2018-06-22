#pragma once

extern float g_cameraSpeed;

extern UInt32 g_targetFormID;
extern double g_targetPosX;
extern double g_targetPosY;
extern double g_targetDist;
extern const char* g_targetName;

namespace Hooks
{
	void Init(void);
}
