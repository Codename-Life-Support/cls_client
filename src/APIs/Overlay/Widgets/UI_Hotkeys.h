#pragma once



void UI_ShowHotkeys(bool _show_dev_menus, bool _show_session_manager) {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);          // no rounded corners
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);        // remove border
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.3f));  // RGBA, last value = transparency

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
	ImGui::Begin("HotKeys", 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);



	ImVec4 green = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
	ImVec4 red = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
	// F7 line
	ImGui::TextColored(_show_dev_menus ? green : red, "F7");
	ImGui::SameLine();
	ImGui::Text(": DevMenu");
	// F8 line
	ImGui::TextColored(_show_session_manager ? green : red, "F8");
	ImGui::SameLine();
	ImGui::Text(": Session");

	ImGui::Text("F9 : Launcher");
	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}