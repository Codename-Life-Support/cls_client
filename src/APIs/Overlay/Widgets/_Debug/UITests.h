#pragma once
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

