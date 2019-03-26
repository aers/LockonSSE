#include "../skse64_common/skse_version.h"
#include "../skse64_common/BranchTrampoline.h"
#include "../skse64/PluginAPI.h"

#include "../skse64/GameData.h"

#include <shlobj.h>

#include "Config.h"
#include "Hooks.h"
#include "Events.h"
#include "Papyrus.h"
#include "Scaleform.h"

IDebugLog gLog;

PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
SKSEScaleformInterface			* g_scaleform = nullptr;
SKSEMessagingInterface			* g_messaging = nullptr;
SKSEPapyrusInterface			* g_papyrus = nullptr;

std::string pluginName;
uint32_t lockonQuestFormID;
uint32_t questListFormID;

void SKSEMessageHandler(SKSEMessagingInterface::Message * message)
{
	switch (message->type)
	{
	case SKSEMessagingInterface::kMessage_DataLoaded:
	{
		Events::Init();
	}
	break;
	default:
		break;
	}
}

extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\LockOnSSE.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
#ifdef _DEBUG
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
#else
		gLog.SetLogLevel(IDebugLog::kLevel_Message);
#endif	

		_MESSAGE("LockOn SSE");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "lockon plugin";
		info->version = 2;

		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");
			return false;
		}
		else if (skse->runtimeVersion != RUNTIME_VERSION_1_5_73)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);
			return false;
		}

		if (!g_branchTrampoline.Create(1024 * 64))
		{
			_FATALERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}

		if (!g_localTrampoline.Create(1024 * 64, nullptr))
		{
			_FATALERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}

		g_messaging = static_cast<SKSEMessagingInterface *>(skse->QueryInterface(kInterface_Messaging));
		if (!g_messaging) {
			_FATALERROR("couldn't get messaging interface");
		}

		g_scaleform = static_cast<SKSEScaleformInterface *>(skse->QueryInterface(kInterface_Scaleform));
		if (!g_scaleform)
		{
			_FATALERROR("couldn't get scaleform interface");
			return false;
		}

		g_papyrus = static_cast<SKSEPapyrusInterface *>(skse->QueryInterface(kInterface_Papyrus));
		if (!g_papyrus)
		{
			_FATALERROR("couldn't get papyrus interface");
			return false;
		}
		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse)
	{
		pluginName = "hmkLockOn.esp";
		lockonQuestFormID = 0x000D62;
		questListFormID = 0x000D64;

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "SKSE", SKSEMessageHandler);

		if (g_scaleform)
		{
		 	g_scaleform->Register("lockon", Scaleform::RegisterCallback);
		}
		if (g_papyrus)
		{
			g_papyrus->Register(Papyrus::Init);
		}
		Hooks::Init();

		return true;
	}
};