#pragma once


GameSessionInfo cached_session = {};

void Debug_ShowGameSessionInfo() {

    ImGui::Begin("Game Session Info");

    if (ImGui::Button("Get new data")) {
        cached_session = GetSessionDetails();
    }


    ImGui::Text("Game:");
    ImGui::SameLine();
    ImGui::Text("%d", static_cast<int>(cached_session.game));

    ImGui::Text("Map Type:");
    ImGui::SameLine();
    ImGui::Text("%d", static_cast<int>(cached_session.type));

    ImGui::Text("Map Name:");
    ImGui::InputText("##mapname", (char*)cached_session.map_name.c_str(), cached_session.map_name.size() + 1, ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Mod Name:");
    ImGui::InputText("##modname", (char*)cached_session.mod_name.c_str(),
        cached_session.mod_name.size() + 1,
        ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Workshop ID:");
    uint64_t id = cached_session.workshop_id;
    ImGui::InputScalar("##workshopid", ImGuiDataType_U64, &id,
        nullptr, nullptr,
        nullptr,
        ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("MapVar Name:");
    ImGui::InputText("##mapvar", (char*)cached_session.mapvar_name.c_str(),
        cached_session.mapvar_name.size() + 1,
        ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("GameVar Name:");
    ImGui::InputText("##gamevar", (char*)cached_session.gamevar_name.c_str(),
        cached_session.gamevar_name.size() + 1,
        ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Gametype Category:");
    ImGui::SameLine();
    ImGui::Text("%d", static_cast<int>(cached_session.gametype_category));


    ImGui::End();
}

