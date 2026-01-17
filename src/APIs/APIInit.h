#pragma once

void injected_window_main() { while (true) { Sleep(30000); } } // keep thread open forever??? possibly redundant?
void OverlayDLLInit() {
    HooksInit();
    InjectedSteamInit();
    std::thread(injected_window_main).detach();
}


