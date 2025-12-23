#pragma once

// other stuff




// bonus other stuff

// even more bonus other stuff
ID3D11RenderTargetView* d3d11RenderTargetView;


bool needs_reloading = true;
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

void InitObjRender(IDXGISwapChain* swap_chain, ID3D11Device1* dx_device, HWND hwnd) {
 
    ReloadViewportStuff(swap_chain, dx_device, hwnd);
}

bool ObjRender(ID3D11DeviceContext1* dx_device_context, IDXGISwapChain* swap_chain, ID3D11Device1* dx_device, HWND hwnd) {
    if (needs_reloading && ReloadViewportStuff(swap_chain, dx_device, hwnd))
        return false;

    dx_device_context->OMSetRenderTargets(1, &d3d11RenderTargetView, 0);
    return true;
}