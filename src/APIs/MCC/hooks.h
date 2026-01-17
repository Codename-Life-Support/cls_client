#pragma once


const unsigned long long D3D11_DrawIndexed_offset = 0x12dc30;
const unsigned long long D3D11_VSSetShader_offset = 0x12a740;
const unsigned long long D3D11_VSSetConstantBuffers_offset = 0x12ad30;
const unsigned long long DXGI_Present_offset = 0x0018c0;
bool HooksInit() {
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
    if (MH_CreateHook(dxgi_present_address, &DX_Render, (void**)(&send_func_ptr)) != MH_OK) {
        MessageBoxA(0, "[CLS-CLIENT] could not set hook at IDXGISwapChain::Present -> dxgi.h", "DirectX hook failure", 0);
        return false;
    }
    if (MH_EnableHook(dxgi_present_address) != MH_OK) {
        MessageBoxA(0, "[CLS-CLIENT] could not enable hook at IDXGISwapChain::Present -> dxgi.h", "DirectX hook failure", 0);
        return false;
    }


    HMODULE networkshipping_module = GetModuleHandleA("halonetworklayer_ship.dll");
    if (!networkshipping_module) {
        MessageBoxA(0, "[CLS-CLIENT] could not get a reference to halonetworklayer_ship.dll", "Network hook failure", 0);
        return false;
    }
    lobby_info = (void**)((long long)(networkshipping_module)+0x16D190);



    HMODULE exeModule = GetModuleHandle(nullptr);
    if (!exeModule) {
        MessageBoxA(0, "[CLS-CLIENT] could not get a reference to parent: MCC shipping exe", "Network hook failure", 0);
        return false;
    }
    char* content_check_address = (char*)exeModule + 0x25157B;
    if (MH_CreateHook(content_check_address, &AsmDetour0, (void**)(&player_content_check_ptr)) != MH_OK) {
        MessageBoxA(0, "[CLS-CLIENT] could not set hook at MCC checkplayercontent", "MCC hook failure", 0);
        return false;
    }
    if (MH_EnableHook(content_check_address) != MH_OK) {
        MessageBoxA(0, "[CLS-CLIENT] could not enable hook at MCC checkplayercontent", "DirectX hook failure", 0);
        return false;
    }

    // game state hooks
    GameState_Hooks((uint64_t)exeModule);

    // MapInfo getters
    session_struct_ptr_ptr = (char**)((uint64_t)exeModule + 0x4001658);
    global_mapinfo_ptr = (char****)((uint64_t)exeModule + 0x3f66780);
    GetMapInfo = (GetMapInfoPtr*)((uint64_t)exeModule + 0x209c4bc);



    return true;
}