#pragma once

#include "../../skse64/PapyrusVM.h"
#include "../../skse64/PapyrusArgs.h"

namespace EventLib
{
	// PapyrusEvents.cpp
	template <typename T>
	void SetVMValue(VMValue* val, T arg)
	{
		VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
		PackValue(val, &arg, registry);
	}

	template <>
	inline void SetVMValue<bool>(VMValue* val, bool arg) { val->SetBool(arg); }

	template <>
	inline void SetVMValue<SInt32>(VMValue* val, SInt32 arg) { val->SetInt(arg); }

	template <>
	inline void SetVMValue<UInt32>(VMValue* val, UInt32 arg) { val->SetInt(arg); }

	template <>
	inline void SetVMValue<float>(VMValue* val, float arg) { val->SetFloat(arg); }

	template <>
	inline void SetVMValue<BSFixedString>(VMValue* val, BSFixedString arg) { val->SetString(arg.data); }

	class EventFunctor0 : public IFunctionArguments
	{
	public:
		EventFunctor0(BSFixedString& a_eventName)
			: eventName(a_eventName.data)
		{
		}

		bool Copy(Output* dst) override
		{
			dst->Resize(0);
			return true;
		}

		void operator()(const UInt64 handle)
		{
			VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
			_DMESSAGE("[DEBUG] Queueing papyrus event with name %s", eventName);
			registry->QueueEvent(handle, &eventName, this);
		}

	private:
		BSFixedString eventName;
	};

#define NUM_PARAMS 1
#include "EventFunctors.inl"

#define NUM_PARAMS 2
#include "EventFunctors.inl"

#define NUM_PARAMS 3
#include "EventFunctors.inl"

#define NUM_PARAMS 4
#include "EventFunctors.inl"

#define NUM_PARAMS 5
#include "EventFunctors.inl"

#define NUM_PARAMS 6
#include "EventFunctors.inl"

#define NUM_PARAMS 7
#include "EventFunctors.inl"

#define NUM_PARAMS 8
#include "EventFunctors.inl"

#define NUM_PARAMS 9
#include "EventFunctors.inl"

#define NUM_PARAMS 10
#include "EventFunctors.inl"
}
