#pragma once



uint32_t launcher_focus_ack = 0;

void FocusLauncher(bool focus_CGB)
{
    // TODO: exception if launcher isn't open??
    // TOOD: and put this logic elsewhere, like inside he IPC loop
    HWND hwnd = FindWindowA(nullptr, "Server Browser");
    if (!hwnd) {
        MessageBoxA(nullptr, "Failed to find Launcher window to focus.", "Overlay fail", MB_OK | MB_ICONWARNING);
        return;
    }
    // write whether to focus the CGB into our ack 
    launcher_focus_ack &= 0xFFFFFFFE;
    launcher_focus_ack |= focus_CGB;
    launcher_focus_ack += 2;
}
