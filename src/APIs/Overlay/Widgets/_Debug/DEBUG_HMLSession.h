#pragma once


void Debug_ShowHMLSession() {
    ImGui::Begin("IPC Recieved");
    string lobbylink = string("Join link: ") + GetJoinLink();
    ImGui::Text(lobbylink.c_str());
    ImGui::Text("Context: %s", IPC_recv_cached.lobby_name);
    ImGui::Text("Summary: %s", IPC_recv_cached.lobby_summary);
    ImGui::Text("ID: %llu", IPC_recv_cached.steam_lobbyid);
    if (IPC_recv_cached.steam_lobbyid == steam_lobbyid) {
        ImGui::Text("It looks like we're in the right session.");
    }
    // TODO: will this ever be needed?
    else if (IPC_recv_cached.steam_lobbyid == -1) {
        ImGui::Text("we're recieving a text notification from the launcher.");
    }
    else {
        ImGui::Text("It looks like we're trying to join into a session.");
    }

    ImGui::Text("Hosted session ID: 0x%016llX", hosted_session_id);
    ImGui::Text("Hosted session Secret: 0x%016llX", hosted_session_secret);
    ImGui::End();
}