#pragma once

to_launcher_struct IPC_send_cached = {};
to_overlay_struct IPC_recv_cached = {};

void IPC_Run() {
	// read any data from our thingo
	if (IPC_PollRead(IPC_recv_cached)) {
		// alert read??
	}

	// we cant IPC if we dont have all the data to send
	PlayerList* player_list = GetPlayersList();
	if (!player_list) return;

	// check all things against cached!!!
	bool has_update = false;
	if (IPC_send_cached.steam_hostid   != steam_userid
	||  IPC_send_cached.steam_lobbyid  != steam_lobbyid
	||  IPC_send_cached.players        != player_list->player_count
	||  IPC_send_cached.max_players    != hosted_session_max_players
	||  IPC_send_cached.hosted_lobbyid != hosted_session_id
	||  IPC_send_cached.focus_launcher != launcher_focus_ack)
	{
		IPC_send_cached.steam_hostid	= steam_userid;
		IPC_send_cached.steam_lobbyid	= steam_lobbyid;
		IPC_send_cached.players			= player_list->player_count;
		IPC_send_cached.max_players		= hosted_session_max_players;
		IPC_send_cached.hosted_lobbyid  = hosted_session_id;
		IPC_send_cached.focus_launcher  = launcher_focus_ack;
		IPC_QueueWrite(IPC_send_cached);
	}

}
