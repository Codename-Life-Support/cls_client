#include "pch.h"

#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <DirectXMath.h>

#include <vector>
#include <map>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <fstream>

#include "res/MinHook/MinHook.h"
#include <thread>


using namespace DirectX;
using namespace std;
#include "UI.h"
#include "Test.h"



// forward declarations
LRESULT CALLBACK WndProcHook2(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void init_graphics(IDXGISwapChain* swap_chain);

// callback related stuff
HWND target_hwnd = 0;
WNDPROC winproc_callback = 0;

// directX info
ID3D11Device1* dx_device = nullptr;
ID3D11DeviceContext1* dx_device_context = nullptr;
IDXGISwapChain* last_swap_chain = nullptr;

// random debugging junk
std::map<void*, int> device_dict;


typedef HRESULT(dx_present_ptr)(IDXGISwapChain* swap_chain, UINT SyncInterval, UINT Flags);
dx_present_ptr* send_func_ptr = NULL;
HRESULT  DLLRun(IDXGISwapChain* swap_chain, UINT SyncInterval, UINT Flags) {
    if (last_swap_chain != swap_chain) {
        MessageBoxA(0, "[CLS-CLIENT] swap chain change detected, resetting", "DirectX debug", 0);
        dx_device_context = 0;
    }
    // init graphics if we haven't yet, and the other hooks have fetched the ptr
    if (!dx_device_context)
        init_graphics(swap_chain);
    //    if (!globals.last_d3d11DeviceContext) return;
    //    else init_graphics(swap_chain, (ID3D11DeviceContext1*)globals.last_d3d11DeviceContext);
    if (!dx_device){  // if no dx_device then our init failed, and our code is inoperable
        MessageBoxA(0, "[CLS-CLIENT] no dx device on render", "DirectX hook failure", 0);
        goto ret;
    }

    // if objrender passes, that means our render target is valid after a resize?? so no crashes during this thing i hope!
    if (ObjRender(dx_device_context, swap_chain, dx_device, target_hwnd)) {
        DrawGUI(dx_device, dx_device_context, target_hwnd);
    }

    ret:
    return send_func_ptr(swap_chain, SyncInterval, Flags);
}






void init_graphics(IDXGISwapChain* swap_chain) {
    last_swap_chain = swap_chain;
    dx_device = 0;
    dx_device_context = (ID3D11DeviceContext1*)1; // make this thing have a value

    // config the target widnow & set windows event hook
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swap_chain->GetDesc(&swapChainDesc);
    target_hwnd = swapChainDesc.OutputWindow;
    winproc_callback = (WNDPROC)SetWindowLongPtrA(target_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook2);
    if (!winproc_callback) {
        MessageBoxA(0, "[CLS-CLIENT] failed to set the new winproc hook", "WinProc hook failure", 0);
        return;
    }

    // Get the D3D11 device
    ID3D11Device* pD3D11Device = nullptr;
    if (FAILED(swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&pD3D11Device))) {
        MessageBoxA(0, "[CLS-CLIENT] failed to get directX device from swap chain", "DirectX hook failure", 0);
        return;
    }
    // Get the ID3D11Device1 interface
    if (FAILED(pD3D11Device->QueryInterface(__uuidof(ID3D11Device1), (void**)(&dx_device)))) {
        MessageBoxA(0, "[CLS-CLIENT] failed to query directX device for version 1", "DirectX hook failure", 0);
        return;
    }

    // Retrieve the device context
    ID3D11DeviceContext* deviceContext;
    dx_device->GetImmediateContext(&deviceContext);
    if (!deviceContext) {
        MessageBoxA(0, "[CLS-CLIENT] failed to get directX device context from directX device", "DirectX hook failure", 0);
        dx_device = 0;
        return;
    }
    if (FAILED(deviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&dx_device_context)))) {
        MessageBoxA(0, "[CLS-CLIENT] failed to query directX device context for version 1", "DirectX hook failure", 0);
        dx_device = 0;
        return;
    }


    InitObjRender(swap_chain, dx_device, target_hwnd);
    init_imgui(dx_device, dx_device_context, target_hwnd);

    MessageBoxA(0, "[CLS-CLIENT] graphics init", "INIT DEBUG", 0);
}


LRESULT CALLBACK WndProcHook2(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return 0;
    //switch (msg) {
    //    //case WM_KEYDOWN:
    //    //    if (wparam == VK_RETURN) {
    //    //        char buff[100];
    //    //        sprintf_s(buff, "number alert: %d", doodybug_number);
    //    //        MessageBoxA(0, buff, "WinProc info", 0);
    //    //    }
    //    //    if (wparam == VK_BACK) {
    //    //        char buff[100];
    //    //        sprintf_s(buff, "injected draw status: %d", drawcount);
    //    //        MessageBoxA(0, buff, "WinProc info", 0);
    //    //    }
    //    //case WM_KEYUP:
    //    //case WM_MOVE:
    //    //    break;
    //case WM_SIZE:
    //    if (last_swap_chain && dx_device)
    //        ReloadViewportStuff(last_swap_chain, dx_device, hwnd);
    //    break;
    //}
    return CallWindowProcA(winproc_callback, hwnd, msg, wparam, lparam);
}


const unsigned long long D3D11_DrawIndexed_offset = 0x12dc30;
const unsigned long long D3D11_VSSetShader_offset = 0x12a740;
const unsigned long long D3D11_VSSetConstantBuffers_offset = 0x12ad30;
const unsigned long long DXGI_Present_offset = 0x0018c0;
bool hook_main() {
    if (MH_Initialize() != MH_OK) {
        MessageBoxA(0, "[CLS-CLIENT] could not initialize MinHook", "DirectX hook failure", 0);
        return false;
    }

    HMODULE dxgi_module = GetModuleHandleA("dxgi.dll"); // d3d11.dll
    if (!dxgi_module) {
        MessageBoxA(0, "[CLS-CLIENT] could not get a reference to dxgi.dll", "DirectX hook failure", 0);
        return false;
    }
    char* dxgi_present_address = (char*)dxgi_module + DXGI_Present_offset;
    if (MH_CreateHook(dxgi_present_address, &DLLRun, (void**)(&send_func_ptr)) != MH_OK) {
        MessageBoxA(0, "[CLS-CLIENT] could not set hook at IDXGISwapChain::Present -> dxgi.h", "DirectX hook failure", 0);
        return false;
    }
    if (MH_EnableHook(dxgi_present_address) != MH_OK) {
        MessageBoxA(0, "[CLS-CLIENT] could not enable hook at IDXGISwapChain::Present -> dxgi.h", "DirectX hook failure", 0);
        return false;
    }
    return true;
}
void injected_window_main() {while (true) {Sleep(30000);}} // keep thread open forever??? possibly redundant?
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH: break;
    case DLL_PROCESS_ATTACH:
        hook_main();
        std::thread(injected_window_main).detach();
        break;
    }
    return TRUE;
}
