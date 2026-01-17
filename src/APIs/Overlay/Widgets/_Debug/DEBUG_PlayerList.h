#pragma once


void Debug_ShowPlayerList() {
    ImGui::Begin("Player debug");
    PlayerList* player_list = GetPlayersList();
    if (player_list) {
        ImGui::Text("players: %I64u, valid players mask: %I64u", player_list->player_count, player_list->player_bitmask);
        ImGui::Text("evil players: %I64u", evil_players);
        static int player_index = 0;
        ImGui::SliderInt("anchor", &player_index, 0, 15);

        // then we check to see if the player is valid vs the validity bitmask
        int bit = 1 << player_index;
        if (bit & player_list->player_bitmask) {

            void* ptr = &player_list->players[player_index];
            char ptr_buf[32];
            snprintf(ptr_buf, sizeof(ptr_buf), "%p", ptr);
            ImGui::InputText("Data Ptr", ptr_buf, sizeof(ptr_buf), ImGuiInputTextFlags_ReadOnly);

            ImGui::Text("gt1: %s", wchar_to_utf8(player_list->players[player_index].gamertag0).c_str());
            ImGui::Text("gt2: %s", wchar_to_utf8(player_list->players[player_index].gamertag1).c_str());
            ImGui::Text("Clan tag: %s", wchar_to_utf8(player_list->players[player_index].clanTag).c_str());
            ImGui::Text("ID: %s", wchar_to_utf8(player_list->players[player_index].serviceId).c_str());
            ImGui::Text("GUID: %s", player_list->players[player_index].guid);

            ImGui::Text("XUID: 0x%016llX", player_list->players[player_index].XUID);
            ImGui::Text("Network ID: 0x%016llX", player_list->players[player_index].networkId);

            ImGui::Text("Flags: 0x%08X | 0x%08X | 0x%08X", player_list->players[player_index].state, player_list->players[player_index].flags0_0, player_list->players[player_index].flags0_1);
            ImGui::Text("Flags: %02X %02X %02X %02X %02X %02X %02X",
                player_list->players[player_index].flags[0], player_list->players[player_index].flags[1], player_list->players[player_index].flags[2], player_list->players[player_index].flags[3],
                player_list->players[player_index].flags[5], player_list->players[player_index].flags[5], player_list->players[player_index].flags[6]);

        }
        else {
            ImGui::Text("player is not valid.");
        }

    }
    else {
        ImGui::Text("Playerlist is not loaded right now. top level ptr: %llu", lobby_info);
    }
    ImGui::End();
}