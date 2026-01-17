#pragma once

static void ShowFlag(const char* label, uint8_t* flagPtr){
    bool value = (flagPtr && *flagPtr != 0);
    ImGui::Checkbox(label, &value);   // visual only, not written back
}

void Debug_ShowGameState() {

    ImGui::Begin("Game State Flags");

    ImGui::Text("SquadPrivacy %d", *SquadPrivacy);
    ShowFlag("IsInMainMenu", IsInMainMenu);
    ShowFlag("IsInMenu", IsInMenu);
    ShowFlag("IsPauseMenu", IsPauseMenu);
    ShowFlag("IsPaused", IsPaused);
    ShowFlag("IsLoading", IsLoading);
    ShowFlag("IsEndingGame", IsEndingGame);
    ShowFlag("IsLeavingGame", IsLeavingGame);
    ShowFlag("IsLoadingBackToMenu", IsLoadingBackToMenu);
    ShowFlag("IsInGame1", IsInGame1);
    ShowFlag("IsInGame2", IsInGame2);
    ShowFlag("IsInGame3", IsInGame3);

    ImGui::End();
}
