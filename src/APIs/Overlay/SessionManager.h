#pragma once

bool recache_host_info_for_IPC = false;

// our current session ID
uint64_t hosted_session_id = 0;
uint64_t hosted_session_secret = 0;
int hosted_session_max_players = -1;

string last_lobby_name;
// cached items that need to be routinely checked to see if our session needs an update
GameSessionInfo last_game_info = {};
string last_map_name;
uint64_t last_steamlobby_id;
int last_player_count = 0;

GameSessionInfo current_request_game_info = {};
string current_request_map_name;
uint64_t current_request_steamlobby_id;
int current_request_player_count = 0;

bool attempting_shutdown = false;
const char* server_publish_url = "http://localhost:3000/api/create";
const char* server_update_url = "http://localhost:3000/api/update";
const char* server_shutdown_url = "http://localhost:3000/api/shutdown";

enum request_type {
	req_routine = 0,
	req_shutdown = 1,
	req_manual_update = 2,
	req_publish = 3,
	req_reconnect = 4
};
request_type last_request_type;
std::unique_ptr<HttpRequest> last_routine_request;

bool IsRequesting() {
	return (last_routine_request.get());
}
bool IsHosting() {
	return (hosted_session_id && hosted_session_secret);
}
void ClearHostingInfo() {
	hosted_session_id = 0;
	hosted_session_secret = 0;
	hosted_session_max_players = -1;
	recache_host_info_for_IPC = true;
}

string cached_sessionid;
string cached_secret;
bool has_cached_credentials = false;
bool HasCachedCredentials() {
	return has_cached_credentials;
}
void SessionManagerInit() {
	has_cached_credentials = ReadSessionValues(cached_sessionid, cached_secret);
	if (has_cached_credentials) {
		PushLog("Reconnect available", "you may attempt to reconnect to regain control of the session you were previously hosting");
	}
}



int routine_fail_count = 0;

// TODO: put this on another thread or something, who knows
auto last_session_update = std::chrono::steady_clock::now();
void UpdateSessionLoop() {

	// if we have a request waiting, lets try to complete it
	HttpRequest* _last_request = last_routine_request.get();
	if (_last_request) {
		if (!_last_request->HasResult()) {
			return;
		}
		long result_code = _last_request->GetResultCode();
		if (result_code == 0) {
			switch (last_request_type) {
			case req_routine: {
				if (routine_fail_count > 4) {
					ClearHostingInfo();
					PushLog("curl routine server update error, abandoning connection", _last_request->GetResult());
				}
				else {
					PushLog("curl routine server update error", _last_request->GetResult());
					routine_fail_count += 1;
				}
				break;
			}
			case req_manual_update: {
				PushLog("curl manual server update error", _last_request->GetResult());
				break;
			}
			case req_shutdown: {
				PushLog("curl shutdown server error", _last_request->GetResult());
				break;
			}
			case req_publish: {
				PushLog("curl publish server error", _last_request->GetResult());
				break;
			}
			case req_reconnect: {
				PushLog("curl reconnect server error", _last_request->GetResult());
				break;
			}
			}
		}
		else {
			bool did_error = (result_code != 200);
			switch (last_request_type) {
			case req_routine: {
				if (did_error) {
					if (routine_fail_count > 4) {
						ClearHostingInfo();
						PushLog("routine server update error, abandoning connection", _last_request->GetResult());
					} 
					else {
						PushLog("routine server update error", _last_request->GetResult());
						routine_fail_count += 1;
					}
				}
				else {
					// success!!!! register all changes to our diffing system
					last_game_info = current_request_game_info;
					last_map_name = current_request_map_name;
					last_steamlobby_id = current_request_steamlobby_id;
					last_player_count = current_request_player_count;
					recache_host_info_for_IPC = true;
					routine_fail_count = 0;
				}
				break;
			}
			case req_manual_update: {
				if (did_error) {
					PushLog("manual server update error", _last_request->GetResult());
				}
				else {
					PushLog("manual server update success", "successfully updated session info");
				}
				break;
			}
			case req_shutdown: {
				if (did_error) {
					PushLog("shutdown server request error", _last_request->GetResult());
				}
				// we dont care if it errored or not, it'll expire on the server's end eventually
				ClearHostingInfo();
				break;
			}
			case req_publish: {
				auto result = _last_request->GetResult();
				if (!did_error) {
					// process response
					size_t pos = result.find('\\');
					if (pos == std::string::npos) {
						PushLog("had error publishing session", "Malformed response recieved, could not delimit recieved ID & secret");
					}
					else {
						std::string left = result.substr(0, pos);
						std::string right = result.substr(pos + 1);

						WriteSessionValues(left, right);
						cached_sessionid = left;
						cached_secret = right;
						has_cached_credentials = true;

						hosted_session_id = std::stoull(left, nullptr, 16);
						hosted_session_secret = std::stoull(right, nullptr, 16);
						recache_host_info_for_IPC = true;
						PushLog("Successfully published session !!!!", "request returned and gave us our correctly formatted ID & secret");
					}
				}
				else {
					PushLog("had error publishing session", result);
				}
				break;
			}
			case req_reconnect: {
				auto result = _last_request->GetResult();
				if (!did_error) {
					hosted_session_id = std::stoull(cached_sessionid, nullptr, 16);
					hosted_session_secret = std::stoull(cached_secret, nullptr, 16);
					recache_host_info_for_IPC = true;
					PushLog("Successfully reconnected to session !", "update request was accepted.");
				}
				else {
					PushLog("had error reconnecting to session, try starting a new one.", result);
					// TODO: only if error 404, clear cache (NOTE: 404 isn't currently implemented on the server's end)
				}
				break;
			}
			}
		}
		_last_request = nullptr;
		last_routine_request.release();
	}

	if ((attempting_shutdown && IsHosting()) || !steam_userid || !steam_lobbyid) {
		return;
	}
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(now - last_session_update).count() < 15){
		return;
	}

	last_session_update = now;
	uint64_t current_steamlobby_id = steam_lobbyid;
	GameSessionInfo current_game_info = GetSessionDetails();

	int current_players = 0;
	PlayerList* player_list = GetPlayersList();
	if (player_list) {
		current_players = player_list->player_count;
	}
	string current_map_name = current_game_info.mapvar_name;
	if (current_map_name.empty()) {
		current_map_name = current_game_info.map_name;
	}

	// if we're not hosting then we just want the current game details to pass to the launcher
	if (!IsHosting()) {
		last_game_info = current_game_info;
		last_map_name = current_map_name;
		last_steamlobby_id = current_steamlobby_id;
		last_player_count = current_players;
		recache_host_info_for_IPC = true;
	}
	else {

		// open Json
		json j;
		j["id"] = to_hex16(hosted_session_id);
		j["secret"] = to_hex16(hosted_session_secret);
		if (current_game_info.gametype_category != last_game_info.gametype_category) {
			j["game_mode"] = std::to_string((int)current_game_info.gametype_category);
		}
		if (current_game_info.gamevar_name != last_game_info.gamevar_name) {
			j["game_variant"] = current_game_info.gamevar_name;
		}
		if (current_game_info.mod_name != last_game_info.mod_name) {
			j["mod_name"] = current_game_info.mod_name;
		}
		if (current_game_info.game != last_game_info.game) {
			j["mod_game_engine"] = std::to_string((int)current_game_info.game);
		}
		if (current_game_info.workshop_id != last_game_info.workshop_id) {
			j["mod_workshop_id"] = std::to_string(current_game_info.workshop_id);
		}
		if (current_players != last_player_count) {
			j["player_count_current"] = std::to_string(current_players);
		}
		if (current_map_name != last_map_name) {
			j["map_variant"] = current_map_name;
		}

		if (current_steamlobby_id != last_steamlobby_id) {
			j["steam_lobbyid"] = std::to_string(current_steamlobby_id);
		}
	
		last_request_type = req_routine;
		last_routine_request = std::make_unique<HttpRequest>(server_update_url, j.dump());

		current_request_game_info = current_game_info;
		current_request_map_name = current_map_name;
		current_request_steamlobby_id = current_steamlobby_id;
		current_request_player_count = current_players;
	}
}


json ParseSession(char* _session_name_buffer, char* _session_desc_buffer, int _max_players, const vector<uint64_t>& _selected_mods) {
	attempting_shutdown = false;
	routine_fail_count = 0;
	last_session_update = std::chrono::steady_clock::now();
	// if we're publishing, we have to disregard our prev tokens if we have any
	hosted_session_id = 0;
	hosted_session_secret = 0;
	// static stuff that we have to intentionally change
	hosted_session_max_players = _max_players;

	std::vector<std::string> modStrings;
	modStrings.reserve(_selected_mods.size());
	for (uint64_t id : _selected_mods) {
		modStrings.push_back(std::to_string(id));
	}

	// stuff that we need to cache so we can check for server updates
	last_steamlobby_id = steam_lobbyid;
	last_game_info = GetSessionDetails();

	last_lobby_name = string(_session_name_buffer);

	json j;

	j["name"] = last_lobby_name;
	j["description"] = string(_session_desc_buffer);
	j["player_count_max"] = std::to_string(hosted_session_max_players);
	j["mods"] = modStrings;

	j["game_mode"] = std::to_string((int)last_game_info.gametype_category);
	j["game_variant"] = last_game_info.gamevar_name;
	j["mod_name"] = last_game_info.mod_name;
	j["mod_game_engine"] = PrintHaloGame(last_game_info.game);
	j["mod_workshop_id"] = std::to_string(last_game_info.workshop_id);

	last_player_count = 0;
	PlayerList* player_list = GetPlayersList();
	if (player_list) {
		last_player_count = player_list->player_count;
	}
	j["player_count_current"] = std::to_string(last_player_count);

	last_map_name = last_game_info.mapvar_name;
	if (last_map_name.empty()) {
		last_map_name = last_game_info.map_name;
	}
	j["map_variant"] = last_map_name;
	

	j["steam_lobbyid"] = std::to_string(last_steamlobby_id);
	j["steam_id"] = std::to_string(steam_userid);
	
	return j;
}
void PublishSession(char* _session_name_buffer, char* _session_desc_buffer, int _max_players, const vector<uint64_t>& _selected_mods) {
	json j = ParseSession(_session_name_buffer, _session_desc_buffer, _max_players, _selected_mods);
	last_request_type = req_publish;
	last_routine_request = std::make_unique<HttpRequest>(server_publish_url, j.dump());
}
void ReconnectSession(char* _session_name_buffer, char* _session_desc_buffer, int _max_players, const vector<uint64_t>& _selected_mods) {
	json j = ParseSession(_session_name_buffer, _session_desc_buffer, _max_players, _selected_mods);
	// dont bother validating whether the strings contain the right data, if someone breaks the code flow then the server will tell them at least that they forgot to give credentials
	j["id"] = cached_sessionid;
	j["secret"] = cached_secret;
	last_request_type = req_reconnect;
	last_routine_request = std::make_unique<HttpRequest>(server_update_url, j.dump());
}

bool ManualSessionUpdate(char* _session_name_buffer, char* _session_desc_buffer, int _max_players, const vector<uint64_t>& _selected_mods) {
	if (!hosted_session_id || !hosted_session_secret || IsRequesting()) {
		return false;
	}
	// static stuff that we have to intentionally change
	hosted_session_max_players = _max_players;
	last_lobby_name = string(_session_name_buffer);

	std::vector<std::string> modStrings;
	modStrings.reserve(_selected_mods.size());
	for (uint64_t id : _selected_mods) {
		modStrings.push_back(std::to_string(id));
	}
	

	json j;
	j["id"] = to_hex16(hosted_session_id);
	j["secret"] = to_hex16(hosted_session_secret);
	j["name"] = last_lobby_name;
	j["description"] = string(_session_desc_buffer);
	j["player_count_max"] = std::to_string(hosted_session_max_players);
	j["mods"] = modStrings;

	last_request_type = req_manual_update;
	last_routine_request = std::make_unique<HttpRequest>(server_update_url, j.dump());
	return true;
}

bool EndSession() {
	if (!hosted_session_id || !hosted_session_secret || IsRequesting()) {
		return false;
	}

	json j;
	j["id"] = to_hex16(hosted_session_id);
	j["secret"] = to_hex16(hosted_session_secret);

	// add it to the thingo list
	attempting_shutdown = true;
	last_request_type = req_shutdown;
	last_routine_request = std::make_unique<HttpRequest>(server_shutdown_url, j.dump());
	return true;
}


