
#define _CRT_SECURE_NO_WARNINGS 
#pragma warning(disable : 4996)
#define WIN32_LEAN_AND_MEAN


#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <DirectXMath.h>

#include <Windows.h>
#include <vector>
#include <map>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <fstream>
#include <filesystem>
#include <map>
#include <wrl/client.h>


#include "res/MinHook/MinHook.h"
#include <thread>
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>
#include <iomanip>
#include <queue>
#include <mutex>

#include "res/json.hpp"
#include "res/curl/curl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "res/stb_image.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
#include "APIs/Overlay/OverlayLog.h"

#include "APIs/DirectX/DX_Textures.h"

#include "APIs/Utility/GuidUtil.h"
#include "APIs/Server/Server.h"

#include "APIs/Steam/UGCParse.h"
#include "APIs/Steam/steam.h"
#include "APIs/Steam/WorkshopPath.h"
#include "APIs/MCC/PlayerList.h"
#include "APIs/MCC/GameState.h"
#include "APIs/Launcher/IPC/IPCSharedEnums.h"
#include "APIs/MCC/GameSession.h"

#include "APIs/Overlay/SessionManagerCache.h"
#include "APIs/Overlay/SessionManager.h"

#include "APIs/Launcher/FocusLauncher.h"
#include "APIs/Launcher/IPC/IPC.h"
#include "APIs/Launcher/IPC/LauncherIPC.h"

#include "APIs/Overlay/UIInit.h"

#include "APIs/Overlay/Widgets/_Debug/DEBUG_GameState.h"
#include "APIs/Overlay/Widgets/_Debug/DEBUG_PlayerList.h"
#include "APIs/Overlay/Widgets/_Debug/DEBUG_SessionStruct.h"
#include "APIs/Overlay/Widgets/_Debug/DEBUG_HMLSession.h"
#include "APIs/Overlay/Widgets/_Debug/DEBUG_GameSession.h"

#include "APIs/Overlay/Widgets/UI_MissingContentPopup.h"
#include "APIs/Overlay/Widgets/UI_ChangedPrivacyPopup.h"
#include "APIs/Overlay/Widgets/UI_PublishedSessionManager.h"
#include "APIs/Overlay/Widgets/UI_Hotkeys.h"

#include "APIs/Overlay/UIRun.h"

#include "APIs/MCC/Inputs.h"

#include "APIs/DirectX/DXInit.h"
#include "APIs/DirectX/DXRun.h"


#include "APIs/MCC/hooks.h"

#include "APIs/APIInit.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH: break;
    case DLL_PROCESS_ATTACH:
        OverlayDLLInit();
        break;
    }
    return TRUE;
}
