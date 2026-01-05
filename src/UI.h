#pragma once
#include "res/imgui/imgui.h"
#include "res/imgui/imgui_impl_win32.h"
#include "res/imgui/imgui_impl_dx11.h"


void init_imgui(ID3D11Device1* _dx_device, ID3D11DeviceContext1* _dx_device_context, HWND _target_hwnd) {
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(_target_hwnd);
    ImGui_ImplDX11_Init(_dx_device, _dx_device_context);
}

struct window_align {
    float x_factor = 10.0f;
    float y_factor = 10.0f;
    int index = 1;
};
static window_align aligns[64] = {};
void handle_align(int index) {
    ImVec2 screen = ImGui::GetIO().DisplaySize;
    auto curr = aligns[index];
    switch (curr.index) {
    case 1: // top left
        ImGui::SetNextWindowPos(ImVec2(curr.x_factor, curr.y_factor), ImGuiCond_Always, ImVec2(0, 0));
        break;
    case 2: // top middle
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f + curr.x_factor, curr.y_factor), ImGuiCond_Always, ImVec2(0.5f, 0));
        break;
    case 3: // top right
        ImGui::SetNextWindowPos(ImVec2(screen.x - curr.x_factor, curr.y_factor), ImGuiCond_Always, ImVec2(1, 0));
        break;
    case 4: // middle left
        ImGui::SetNextWindowPos(ImVec2(curr.x_factor, screen.y * 0.5f + curr.y_factor), ImGuiCond_Always, ImVec2(0, 0.5f));
        break;
    case 5: // center
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f + curr.x_factor, screen.y * 0.5f + curr.y_factor), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        break;
    case 6: // middle right
        ImGui::SetNextWindowPos(ImVec2(screen.x - curr.x_factor, screen.y * 0.5f + curr.y_factor), ImGuiCond_Always, ImVec2(1, 0.5f));
        break;
    case 7: // bottom left
        ImGui::SetNextWindowPos(ImVec2(curr.x_factor, screen.y - curr.y_factor), ImGuiCond_Always, ImVec2(0, 1));
        break;
    case 8: // bottom middle
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f + curr.x_factor, screen.y - curr.y_factor), ImGuiCond_Always, ImVec2(0.5f, 1));
        break;
    case 9: // bottom right
        ImGui::SetNextWindowPos(ImVec2(screen.x - curr.x_factor, screen.y - curr.y_factor), ImGuiCond_Always, ImVec2(1, 1));
        break;
    default:
        // optional: fallback to center
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, screen.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        break;
    }
}


void ctrl_window() {
    ImGui::Begin("Sample copy window");
    static int curr_wind = 0;
    ImGui::SliderInt("window index", &curr_wind, 0, 63);
    ImGui::SliderFloat("x_offs", &aligns[curr_wind].x_factor, 0.0f, 1000.0f);
    ImGui::SliderFloat("y_offs", &aligns[curr_wind].x_factor, 0.0f, 1000.0f);
    ImGui::SliderInt("anchor", &aligns[curr_wind].index, 1, 9);
    bool selected[9] = { false };
    selected[aligns[curr_wind].index - 1] = true; // mark current selection
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int idx = row * 3 + col + 1;
            ImGui::PushID(idx);
            bool value = selected[idx - 1];
            if (ImGui::Checkbox("##chk", &value))
                aligns[curr_wind].index = idx;
            ImGui::PopID();
            if (col < 2)
                ImGui::SameLine();
        }
    }
    ImGui::End();
}


#include <codecvt>
std::string ToUtf8(const std::wstring& w) { std::wstring_convert<std::codecvt_utf8<wchar_t>> conv; return conv.to_bytes(w); }

void DrawGUI(ID3D11Device1* dx_device, ID3D11DeviceContext1* dx_device_context, HWND target_hwnd) {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    ctrl_window();



    handle_align(0);
    ImGui::Begin("Config UIs");
    string lobbylink = string("Join link: ") + GetJoinLink();
    ImGui::Text(lobbylink.c_str());
    ImGui::End();

    handle_align(2);
    ImGui::Begin("Player debug");
    if (player_list) {
        ImGui::Text("players: %I64u, valid players mask: %I64u", player_list->player_count, player_list->player_bitmask);
        ImGui::Text("evil players: %I64u", evil_players);
        static int player_index = 0;
        ImGui::SliderInt("anchor", &player_index, 0, 15);

        // then we check to see if the player is valid vs the validity bitmask
        int bit = 1 << player_index;
        if (bit & player_list->player_bitmask) {

            ImGui::Text("gt1: %s", ToUtf8(player_list->players[player_index].gamertag0).c_str());
            ImGui::Text("gt2: %s", ToUtf8(player_list->players[player_index].gamertag1).c_str());
            ImGui::Text("Clan tag: %s", ToUtf8(player_list->players[player_index].clanTag).c_str());
            ImGui::Text("ID: %s", ToUtf8(player_list->players[player_index].serviceId).c_str());
            ImGui::Text("GUID: %s", player_list->players[player_index].webrtcGuid);

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
    ImGui::End();

    handle_align(1);
    ImGui::ShowDemoWindow(nullptr);


    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}