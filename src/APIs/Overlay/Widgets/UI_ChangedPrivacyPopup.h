#pragma once


bool UI_PrivacyChangedAlert = false;

void UI_CheckForPrivacyPopup() {


	if (GameState_EnforceOpenPrivacy()) {
		UI_PrivacyChangedAlert = true;
	}


	if (UI_PrivacyChangedAlert) {

		ImVec2 screen = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos(ImVec2(screen.x * 0.5f, screen.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(400, 250));

		ImGui::Begin("WARNING##2", &UI_PrivacyChangedAlert, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::Text("MCC Squad privacy settings have been updated-");
		ImGui::Text("-to enable modded CGB joining functionality.");
		ImGui::NewLine();
		ImGui::Text("[changed: invite-only to friends-only]");
		ImGui::NewLine();
		ImGui::Text("This change may be saved after restart");
		if (ImGui::Button("Close")) {
			UI_PrivacyChangedAlert = false;
		}
		ImGui::End();
	}
}