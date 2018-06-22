#pragma once

#include "../../skse64/GameCamera.h"

// updated skse definitions live in TES namespace to not conflict

namespace TES
{
	// 1.5.39 ptr 1E16A10
	class TESCameraController
	{
	public:
		TESCameraController() {}

		UInt32 unk00;
		float  startRotZ; // 04
		float  startRotX; // 08
		float  endRotZ;   // 0C
		float  endRotX;   // 10
		float  unk14;     // 14
		float  unk18;     // 18
		UInt8  unk1C;     // 1C
		UInt8  pad1D[3];  // 1D

		void Rotate(float startRotZ, float startRotX, float endRotZ, float endRotX, float fWait, float arg2);
	};

	// 90
	class FirstPersonState : public TESCameraState
	{
	public:
		FirstPersonState();
		virtual ~FirstPersonState();

		PlayerInputHandler		inputHandler;	// 20
		float					unk30[6];		// 30 
		uint64_t				unk48;			// 48
		uint64_t				unk50;			// 50
		NiNode*					cameraNode;		// 58
		NiNode*					controllerNode;	// 60
		float					unk68[3];		// 70
		uint32_t				unk74;			// 74
		uint32_t				unk78;			// 78
		float					unk7C;			// 7C
		uint32_t				unk80;			// 80
		uint8_t					unk84[4];		// 84
		uint64_t				unk88;			// 88
	};
	STATIC_ASSERT(sizeof(FirstPersonState) == 0x90);

	// E8
	class ThirdPersonState : public TESCameraState
	{
	public:
		ThirdPersonState();
		virtual ~ThirdPersonState();
		virtual void Unk_09(void);
		virtual void Unk_0A(void);
		virtual void UpdateMode(bool weaponDrawn);

		PlayerInputHandler		inputHandler;				// 20
		NiNode					* cameraNode;				// 30
		NiNode					* controllerNode;			// 38
		float					unk40[4];					// 40
		uint32_t				unk50[3];					// 50
		float					fOverShoulderPosX;			// 5C
		float					fOverShoulderCombatAddY;	// 60
		float					fOverShoulderPosZ;			// 64
		float					unk68[3];					// 68
		uint32_t				unk74[6];					// 74
		float					unk8C;						// 8C - init'd 7F7FFFFF
		uint32_t				unk90[3];					// 90
		float					unk9C;						// 9C - init'd 7F7FFFFF
		uint64_t				unkA0;						// A0
		uint64_t				unkA8;						// A8
		float					unkB0;						// B0
		uint32_t				unkB4[3];					// B4
		float					unkC0;						// C0
		float					unkC4;						// C4
		float					unkC8;						// C8
		float					unkCC;						// CC
		float					unkD0;						// D0
		float					diffRotZ;					// D4
		float					diffRotX;					// D8
		uint8_t					unkDC[7];					// DC 
		uint8_t					padE3[5];					// E3
	};
	STATIC_ASSERT(sizeof(ThirdPersonState) == 0xE8);
}