#if NUM_PARAMS > 10
#error EventFunctors: too many params
#endif

template <

#if NUM_PARAMS >= 1
	typename T_Arg0
#endif
#if NUM_PARAMS >= 2
	, typename T_Arg1
#endif
#if NUM_PARAMS >= 3
	, typename T_Arg2
#endif
#if NUM_PARAMS >= 4
	, typename T_Arg3
#endif
#if NUM_PARAMS >= 5
	, typename T_Arg4
#endif
#if NUM_PARAMS >= 6
	, typename T_Arg5
#endif
#if NUM_PARAMS >= 7
	, typename T_Arg6
#endif
#if NUM_PARAMS >= 8
	, typename T_Arg7
#endif
#if NUM_PARAMS >= 9
	, typename T_Arg8
#endif
#if NUM_PARAMS >= 10
	, typename T_Arg9
#endif

>

class CLASS_NAME : public IFunctionArguments
{
public:
	CLASS_NAME(BSFixedString& a_eventName
#if NUM_PARAMS >= 1
	           , T_Arg0 a_arg0
#endif
#if NUM_PARAMS >= 2
		, T_Arg1 a_arg1
#endif
#if NUM_PARAMS >= 3
		, T_Arg2 a_arg2
#endif
#if NUM_PARAMS >= 4
		, T_Arg3 a_arg3
#endif
#if NUM_PARAMS >= 5
		, T_Arg4 a_arg4
#endif
#if NUM_PARAMS >= 6
		, T_Arg5 a_arg5
#endif
#if NUM_PARAMS >= 7
		, T_Arg6 a_arg6
#endif
#if NUM_PARAMS >= 8
		, T_Arg7 a_arg7
#endif
#if NUM_PARAMS >= 9
		, T_Arg8 a_arg8
#endif
#if NUM_PARAMS >= 10
		, T_Arg9 a_arg9
#endif
	) : eventName(a_eventName.data)
#if NUM_PARAMS >= 1
	    , arg0(a_arg0)
#endif
#if NUM_PARAMS >= 2
		, arg1(a_arg1)
#endif
#if NUM_PARAMS >= 3
		, arg2(a_arg2)
#endif
#if NUM_PARAMS >= 4
		, arg3(a_arg3)
#endif
#if NUM_PARAMS >= 5
		, arg4(a_arg4)
#endif
#if NUM_PARAMS >= 6
		, arg5(a_arg5)
#endif
#if NUM_PARAMS >= 7
		, arg6(a_arg6)
#endif
#if NUM_PARAMS >= 8
		, arg7(a_arg7)
#endif
#if NUM_PARAMS >= 9
		, arg8(a_arg8)
#endif
#if NUM_PARAMS >= 10
		, arg9(a_arg9)
#endif
	{
	}

	bool Copy(Output* dst) override
	{
		dst->Resize(NUM_PARAMS);
#if NUM_PARAMS >= 1
		SetVMValue(dst->Get(0), arg0);
#endif
#if NUM_PARAMS >= 2
		SetVMValue(dst->Get(1), arg1);
#endif
#if NUM_PARAMS >= 3
		SetVMValue(dst->Get(2), arg2);
#endif
#if NUM_PARAMS >= 4
		SetVMValue(dst->Get(3), arg3);
#endif
#if NUM_PARAMS >= 5
		SetVMValue(dst->Get(4), arg4);
#endif
#if NUM_PARAMS >= 6
		SetVMValue(dst->Get(5), arg5);
#endif
#if NUM_PARAMS >= 7
		SetVMValue(dst->Get(6), arg6);
#endif
#if NUM_PARAMS >= 8
		SetVMValue(dst->Get(7), arg7);
#endif
#if NUM_PARAMS >= 9
		SetVMValue(dst->Get(8), arg8);
#endif
#if NUM_PARAMS >= 10
		SetVMValue(dst->Get(9), arg9);
#endif
		return true;
	}

	void operator()(uint64_t handle)
	{
		VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();
		_DMESSAGE("[DEBUG] Queueing papyrus event with name %s", eventName);
		registry->QueueEvent(handle, &eventName, this);
	}

private:
	BSFixedString eventName;
#if NUM_PARAMS >= 1
	T_Arg0 arg0;
#endif
#if NUM_PARAMS >= 2
		T_Arg1		arg1;
#endif
#if NUM_PARAMS >= 3
		T_Arg2		arg2;
#endif
#if NUM_PARAMS >= 4
		T_Arg3		arg3;
#endif
#if NUM_PARAMS >= 5
		T_Arg4		arg4;
#endif
#if NUM_PARAMS >= 6
		T_Arg5		arg5;
#endif
#if NUM_PARAMS >= 7
		T_Arg6		arg6;
#endif
#if NUM_PARAMS >= 8
		T_Arg7		arg7;
#endif
#if NUM_PARAMS >= 9
		T_Arg8		arg8;
#endif
#if NUM_PARAMS >= 10
		T_Arg9		arg9;
#endif
};
