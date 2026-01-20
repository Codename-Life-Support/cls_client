#pragma once


// callback related stuff
HWND target_hwnd = 0;
// directX info
ID3D11Device1* dx_device = nullptr;
ID3D11DeviceContext1* dx_device_context = nullptr;
IDXGISwapChain* last_swap_chain = nullptr;

// other stuff
bool needs_reloading = true;
ID3D11RenderTargetView* d3d11RenderTargetView;

bool ReloadViewportStuff(IDXGISwapChain* swap_chain, ID3D11Device1* dx_device, HWND hwnd) {
    if (d3d11RenderTargetView) d3d11RenderTargetView->Release();
    needs_reloading = false;

    ID3D11Texture2D* backbuffer;
    if (FAILED(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer))) {
        MessageBoxA(0, "[CLS-CLIENT] swap chain get buffer during resize failed", "DirectX ERROR", 0);
        needs_reloading = true;
        goto ret;
    }
    if (FAILED(dx_device->CreateRenderTargetView(backbuffer, nullptr, &d3d11RenderTargetView))) {
        MessageBoxA(0, "[CLS-CLIENT] swap chain create render target view during resize failed", "DirectX ERROR", 0);
        needs_reloading = true;
    }
ret:
    backbuffer->Release();
    return needs_reloading;
}


void DX_HookedInit(IDXGISwapChain* swap_chain) {
    last_swap_chain = swap_chain;
    dx_device = 0;
    dx_device_context = (ID3D11DeviceContext1*)1; // make this thing have a value

    // config the target widnow & set windows event hook
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swap_chain->GetDesc(&swapChainDesc);
    target_hwnd = swapChainDesc.OutputWindow;

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

    OverlayIPC_Init();
    Inputs_Init(target_hwnd);
    ReloadViewportStuff(swap_chain, dx_device, target_hwnd);
    DX_TexturesInit(dx_device, dx_device_context);
    SessionManagerInit();
    UI_Init(dx_device, dx_device_context, target_hwnd);


    MessageBoxA(0, "[CLS-CLIENT] new graphics init", "INIT DEBUG", 0);
}
bool DX_EnsureInit(IDXGISwapChain* swap_chain) {
    if (!dx_device_context && last_swap_chain != swap_chain) {
        MessageBoxA(0, "[CLS-CLIENT] swap chain change detected, resetting", "DirectX debug", 0);
        dx_device_context = 0;
    }
    if (!dx_device_context) {
        DX_HookedInit(swap_chain);
    }
    if (!dx_device) {  // if no dx_device then our init failed, and our code is inoperable
        MessageBoxA(0, "[CLS-CLIENT] no dx device on render", "DirectX hook failure", 0);
        return false;
    }
    return true;
}
