#pragma once

static volatile bool g_shutdownRequested = false;

void injected_window_main() {
    while (!g_shutdownRequested) {
        Sleep(100);
    }
}

void OverlayDLLInit() {
    g_shutdownRequested = false;
    HooksInit();
    InjectedSteamInit();
    std::thread(injected_window_main).detach();
}

void OverlayDLLShutdown() {
    g_shutdownRequested = true;
    Sleep(200);
    HooksShutdown();
}
