#pragma once

to_launcher_struct IPC_send_cached = {};
to_overlay_struct IPC_recv_cached = {};


void str_copy(char* buffer, int size, const char* src) {
	strncpy(buffer, src, size);
	buffer[size - 1] = '\0';   // ensure null termination
}

void IPC_Run() {
	// read any data from our thingo
	if (IPC_PollRead(IPC_recv_cached)) {
		// alert read??
	}

	// we cant IPC if we dont have all the data to send
	PlayerList* player_list = GetPlayersList();
	if (!player_list) return;



	// check all things against cached!!!
	if (IPC_send_cached.steam_hostid   != steam_userid
	||  IPC_send_cached.steam_lobbyid  != steam_lobbyid
	||  IPC_send_cached.players        != player_list->player_count
	||  IPC_send_cached.max_players    != hosted_session_max_players
	||  IPC_send_cached.hosted_lobbyid != hosted_session_id
	||  IPC_send_cached.focus_launcher != launcher_focus_ack
	||  recache_host_info_for_IPC)
	{
		IPC_send_cached.steam_hostid	= steam_userid;
		IPC_send_cached.steam_lobbyid	= steam_lobbyid;
		IPC_send_cached.players			= player_list->player_count;
		IPC_send_cached.max_players		= hosted_session_max_players;
		IPC_send_cached.hosted_lobbyid  = hosted_session_id;
		IPC_send_cached.focus_launcher  = launcher_focus_ack;
		recache_host_info_for_IPC = false;

		if (IsHosting()) {
			IPC_send_cached.game_engine = last_game_info.game;
			IPC_send_cached.gamemode_category = last_game_info.gametype_category;
			str_copy(IPC_send_cached.mod_name, 64, last_game_info.mod_name.c_str());
			str_copy(IPC_send_cached.map_name, 64, last_map_name.c_str());
			str_copy(IPC_send_cached.mode_name, 64, last_game_info.gamevar_name.c_str());
			str_copy(IPC_send_cached.lobby_name, 64, last_lobby_name.c_str());
		}
		else {
			IPC_send_cached.game_engine = last_game_info.game;
			IPC_send_cached.gamemode_category = last_game_info.gametype_category;
			str_copy(IPC_send_cached.mod_name, 64, last_game_info.mod_name.c_str());
			str_copy(IPC_send_cached.map_name, 64, last_map_name.c_str());
			str_copy(IPC_send_cached.mode_name, 64, last_game_info.gamevar_name.c_str());
			IPC_send_cached.lobby_name[0] = 0;
		}

		IPC_QueueWrite(IPC_send_cached);
	}

}
