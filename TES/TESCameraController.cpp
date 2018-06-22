#include "GameCamera.h"

// Dunno exactly what this is yet
RelocPtr<float> g_UnkFloat(0x02F92950);

// Based off LE implementation of TESCameraController::Rotate (0x00736480), since the function doesn't exist in SE
// It was inlined into the function at 0x006A2ED0 (1.5.39), starting around 0x6A3122 (1.5.39)
// SE function has some additional use of ini settings but we're ignoring that for our purposes 
void TES::TESCameraController::Rotate(float startRotZ, float startRotX, float endRotZ, float endRotX, float fWait, float arg2)
{
	this->startRotZ = startRotZ;
	this->startRotX = startRotX;
	this->endRotZ = endRotZ;
	this->endRotX = endRotX;
	this->unk14 = fWait;
	this->unk18 = *g_UnkFloat - (fWait * arg2);
	this->unk1C = 0;
}
