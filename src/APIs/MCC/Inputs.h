#pragma once


WNDPROC winproc_callback = 0;
// forward declarations
LRESULT CALLBACK Inputs_Hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Inputs_Hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    bool consume_input = false;
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        consume_input = true;
    }
    else if (io.WantTextInput && (msg == WM_KEYDOWN || msg == WM_KEYUP))
    {
        consume_input = true;
    }
    else if (io.WantCaptureMouse && (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST))
    {
        consume_input = true;
    }


    switch (msg) {
        case WM_KEYDOWN:
            switch (wparam) {
            case VK_F7:
                show_dev_menus = !show_dev_menus;
                break;
            case VK_F8:
                show_session_manager = !show_session_manager;
                break;
            case VK_F9:
                FocusLauncher(false);
                break;
            case VK_ESCAPE:
                if (current_overlay_state != overlay_closed) {
                    return 1;
                }
                break;
            }
            break;
        case WM_KEYUP:
            switch (wparam) {
            case VK_ESCAPE:
                if (current_overlay_state == overlay_mods) {
                    current_overlay_state = overlay_details;
                    return 1;
                }
                else if (current_overlay_state == overlay_details) {
                    current_overlay_state = overlay_closed;
                    return 1;
                }
                break;
            }
            break;
        case WM_MOVE:
            break;
    //case WM_SIZE:
    //    if (last_swap_chain && dx_device)
    //        ReloadViewportStuff(last_swap_chain, dx_device, hwnd);
    //    break;
    }
    if (consume_input) return 1;
    return CallWindowProcA(winproc_callback, hwnd, msg, wparam, lparam);
}

void Inputs_Init(HWND target_hwnd) {
    winproc_callback = (WNDPROC)SetWindowLongPtrA(target_hwnd, GWLP_WNDPROC, (LONG_PTR)Inputs_Hook);
    if (!winproc_callback) {
        MessageBoxA(0, "[CLS-CLIENT] failed to set the new winproc hook", "WinProc hook failure", 0);
    }
}