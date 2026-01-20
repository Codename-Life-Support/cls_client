#pragma once



bool show_dev_menus = false;
bool show_session_manager = false;

void UI_Run(ID3D11Device1* dx_device, ID3D11DeviceContext1* dx_device_context, HWND target_hwnd) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI_ShowHotkeys(show_dev_menus, show_session_manager);

    // TODO: make debug menu keybind
    if (show_dev_menus) {
        Debug_ShowHMLSession();
        Debug_ShowGameState();
        Debug_ShowPlayerList();
        Debug_ShowSessionStruct();
        Debug_ShowGameSessionInfo();
    }

    UI_CheckMissingContent();
    // TODO: wait 5 seconds of wrong privacy before taking action?
    UI_CheckForPrivacyPopup(); // TODO: only do this for when attempting to host a session

    UI_ShowSessionManager(show_session_manager);
    UpdateSessionLoop();
    UI_ShowErrors();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}