#pragma once







typedef HRESULT(dx_present_ptr)(IDXGISwapChain* swap_chain, UINT SyncInterval, UINT Flags);
dx_present_ptr* send_func_ptr = NULL;
HRESULT  DX_Render(IDXGISwapChain* swap_chain, UINT SyncInterval, UINT Flags) {
    try {
        if (DX_EnsureInit(swap_chain)) {






            if (!needs_reloading || !ReloadViewportStuff(swap_chain, dx_device, target_hwnd))
            {
                IPC_Run();

                dx_device_context->OMSetRenderTargets(1, &d3d11RenderTargetView, 0);
                UI_Run(dx_device, dx_device_context, target_hwnd);
            }






        }
    }
    catch (const std::exception& ex)
    {
        MessageBoxA(
            nullptr,
            ex.what(),
            "Exception Thrown from overlay!!",
            MB_OK | MB_ICONERROR
        );
    }

    return send_func_ptr(swap_chain, SyncInterval, Flags);
}