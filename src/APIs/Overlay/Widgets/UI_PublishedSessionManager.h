#pragma once

enum overlay_state {
    overlay_closed = 0,
    overlay_details = 1,
    overlay_mods = 2,
}; 
overlay_state current_overlay_state;


vector<WorkshopMod> cached_mods = {};
vector<uint64_t> selected_mods = {};
vector<bool> currently_selected = {};
int count_selected = 0;

char session_name_buffer[64] = {};
char session_desc_buffer[256] = {};
int max_players = 16;


vector<uint64_t> GetSelected() {
    vector<uint64_t> save_selected = {};
    if (currently_selected.size() == cached_mods.size()) {
        for (int i = 0; i < currently_selected.size(); i++) {
            if (currently_selected[i]) {
                save_selected.push_back(cached_mods[i].workshop_id);
            }
        }
    }
    return save_selected;
}

void UI_ShowSessionManager(bool is_window_open) {

    if (IsRequesting()) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.3f));
        ImVec2 screen = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, 30.0f), ImGuiCond_Always, ImVec2(0.5f, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 100));
        ImGui::Begin("Session Request loading", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);
        ImGui::Text("Session request is loading...");
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
    else if (!GameState_IsInSomeMenu() || !is_window_open) {
        current_overlay_state = overlay_closed;
        return;
    }
    // show mod checklist for session details
    else if (current_overlay_state == overlay_mods) {
        ImVec2 screen = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, 30.0f), ImGuiCond_Always, ImVec2(0.5f, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);
        ImGui::Begin("Select Mods", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::PushFont(style_bigFont);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::Text("Select Mods:");
        ImGui::PopStyleColor();
        ImGui::PopFont();

        count_selected = 0;
        for (size_t i = 0; i < cached_mods.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnWidth(0, 60);
            ImGui::Text(UnparseEngine(cached_mods[i].engine)); // Game version
            ImGui::NextColumn();
            ImGui::Text(cached_mods[i].name.c_str()); // Mod name
            ImGui::NextColumn();
            bool is_checked = currently_selected[i];
            ImGui::Checkbox("", &is_checked); // Checkbox
            currently_selected[i] = is_checked;
            count_selected += is_checked;
            ImGui::Columns(1);
            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::Spacing();

        float buttonWidth = (ImGui::GetContentRegionAvail().x - 10.0f) * 0.5f;

        if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
            current_overlay_state = overlay_details;
        }

        ImGui::End();
    }
    // show mod detail editing window 
    else if (current_overlay_state == overlay_details) {
        ImVec2 screen = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, 30.0f), ImGuiCond_Always, ImVec2(0.5f, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 200));
        ImGui::Begin("Session Manager", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::PushFont(style_bigFont);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::Text("Lobby Settings");
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::InputText("SessionName", session_name_buffer, sizeof(session_name_buffer));
        ImGui::InputText("Description", session_desc_buffer, sizeof(session_desc_buffer));

        ImGui::PushItemWidth(-1);
        // every time we open the mods menu, it should refresh the cached mods
        // this is so users can reasonably expect to access mods that installed since they last opened the menu
        // realistically we do this because we're not going to setup a callback 
        char buf[64];
        snprintf(buf, sizeof(buf), "Mods (%d)", count_selected);
        if (ImGui::Button(buf)) {
            current_overlay_state = overlay_mods;

            vector<uint64_t> save_selected = GetSelected();
            cached_mods = Steam_GetInstalledMods();
            // recalculate vector of selected mods
            currently_selected.clear();
            currently_selected.resize(cached_mods.size(), false);
            for (int i = 0; i < cached_mods.size(); i++) {
                uint64_t current = cached_mods[i].workshop_id;
                for (int j = 0; j < save_selected.size(); j++) {
                    if (save_selected[j] == current) {
                        currently_selected[i] = true;
                        break;
                    }
                }
            }
        }
        ImGui::PopItemWidth();
        ImGui::SliderInt("Max Players", &max_players, 1, 16);

        if (IsHosting()) {
            if (ImGui::Button("Update")) {
                selected_mods = GetSelected();
                ManualSessionUpdate(session_name_buffer, session_desc_buffer, max_players, selected_mods);
                current_overlay_state = overlay_closed;
            }
        }
        else if (ImGui::Button("Publish")) {
            selected_mods = GetSelected();
            PublishSession(session_name_buffer, session_desc_buffer, max_players, selected_mods);
            current_overlay_state = overlay_closed;
        }
        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            current_overlay_state = overlay_closed;
        }
        ImGui::End();
    }
    // show the halo mods context widget
    else {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);          // no rounded corners
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);        // remove border
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));  // RGBA, last value = transparency

        ImVec2 screen = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, 30.0f), ImGuiCond_Always, ImVec2(0.5f, 0));
        ImGui::Begin("HaloModsHotbar", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::PushStyleColor(ImGuiCol_Button, style_bg);
        //if (ImGui::Button("HM")){
        //    // Focus halo mods launcher app?
        //}
        if (ImGui::ImageButton( "test",
            halo_mods_image.Get(),        // ImTextureID
            ImVec2(16, 16),               // size of the button
            ImVec2(0, 0),                 // UV top-left
            ImVec2(1, 1),                 // UV bottom-right
            ImVec4(0, 0, 0, 0),              // background color
            ImVec4(1, 1, 1, 1)               // tint color
        ))
        {
            // Button was clicked
            FocusLauncher(false);
        }

        ImGui::SameLine();

        if (!IsHosting()) {
            if (ImGui::Button("Host A Game")) {
                current_overlay_state = overlay_details;
            }
            ImGui::SameLine();
            if (ImGui::Button("Game Browser")) {
                // focus halo mods launcher app CGB
                FocusLauncher(true);
            }
        }
        else {
            if (ImGui::Button("Lobby Settings")) {
                current_overlay_state = overlay_details;
            }
            ImGui::SameLine();
            if (ImGui::Button("End Lobby")) {
                EndSession();
                current_overlay_state = overlay_closed;
            }
        }
        ImGui::PopStyleColor();
        
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}



const char* last_routine_error = nullptr;
const char* last_routine_error_context = nullptr;
void UI_SessionPoll() {
    UpdateSessionLoop(&last_routine_error, &last_routine_error_context);

    if (last_routine_error) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.3f));
        ImVec2 screen = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, 90.0f), ImGuiCond_Always, ImVec2(0.5f, 0));
        ImGui::SetNextWindowSize(ImVec2(400, 100));
        ImGui::Begin("RoutineUpdateFail", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::Text("Routine session update has failed:");
        ImGui::Text(last_routine_error_context);
        ImGui::Text(last_routine_error);
        if (ImGui::Button("OK")) {
            last_routine_error = nullptr;
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }
}