#pragma once



uint32_t launcher_focus_ack = 0;

void BringToFront(HWND hwnd)
{
    if (!IsWindow(hwnd))
        return;

    // Restore if minimized
    if (IsIconic(hwnd))
        ShowWindow(hwnd, SW_RESTORE);

    // Allow SetForegroundWindow
    DWORD fgThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);

    AttachThreadInput(targetThread, fgThread, TRUE);

    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
    SetActiveWindow(hwnd);

    AttachThreadInput(targetThread, fgThread, FALSE);
}
void FocusLauncher(bool focus_CGB)
{
    // TODO: for now we're going to focus the launcher from here
    //       this might be a bit more responsive but we'll see i guess
    HWND hwnd = FindWindowA(nullptr, "Server Browser");
    if (!hwnd) {
        MessageBoxA(nullptr, "Failed to find Launcher window to focus.", "Overlay fail", MB_OK | MB_ICONWARNING);
        return;
    }
    BringToFront(hwnd);
    // write whether to focus the CGB into our ack 
    launcher_focus_ack &= 0xFFFFFFFE;
    launcher_focus_ack |= focus_CGB;
    launcher_focus_ack += 2;
}
