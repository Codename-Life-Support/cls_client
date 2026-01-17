#pragma once


uint64_t cached_players_mask = 0;
bool UI_MissingContentAlert = false;

void UI_CheckMissingContent() {

	uint64_t players_mask;
	if (GetStartAck(&players_mask)) {
		cached_players_mask = players_mask;
		UI_MissingContentAlert = true;
	}

	if (UI_MissingContentAlert){

		ImVec2 screen = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, screen.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(400, 250));

		ImGui::Begin("WARNING##1", &UI_MissingContentAlert, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		if (cached_players_mask) {
			PlayerList* players = GetPlayersList();

			ImGui::Text("Players are missing content:");
			for (int i = 1; i < 16; i++) {
				int bit = 1 << i;
				ImGui::PushID(i);
				if ((cached_players_mask & bit) != 0) {
					ImGui::Text(wchar_to_utf8(std::wstring(players->players[i].gamertag0)).c_str());
				}
				ImGui::PopID();
			}
		}
		else {
			ImGui::Text("All players have content installed...");
			ImGui::Text("(this window shouldn't be seen right now)");
		}
		if (ImGui::Button("Close")) {
			UI_MissingContentAlert = false;
		}
		ImGui::End();
	}
}