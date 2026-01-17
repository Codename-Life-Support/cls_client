#pragma once



// our current session ID
uint64_t hosted_session_id = 0;
uint64_t hosted_session_secret = 0;
int hosted_session_max_players = -1;

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
	req_publish = 3
};
request_type last_request_type;
std::unique_ptr<HttpRequest> last_routine_request;

bool IsRequesting() {
	return (last_routine_request.get());
}
bool IsHosting() {
	return (hosted_session_id && hosted_session_secret);
}

// TODO: put this on another thread or something, who knows
auto last_session_update = std::chrono::steady_clock::now();
void UpdateSessionLoop(const char** error_ptr, const char** error_context_ptr) {

	// if we have a request waiting, lets try to complete it
	HttpRequest* _last_request = last_routine_request.get();
	if (_last_request) {
		if (!_last_request->HasResult()) {
			MessageBoxA(0, "Failed waiting for result", "epic fial", MB_OK);
			return;
		}

		const char* error_msg = _last_request->GetResultError();
		switch (last_request_type) {
		case req_routine: {
			if (error_msg) {
				*error_ptr = error_msg;
				// TODO: implement some sort of response to enable this functionality if its ever needed
				bool server_closed = false;
				if (server_closed) {
					hosted_session_id = 0;
					hosted_session_secret = 0;
					hosted_session_max_players = -1;
					*error_context_ptr = "routine server update error, our server has been forcibly closed.";
				} 
				else {
					*error_context_ptr = "routine server update error";
				}
			}
			else {
				// success!!!! register all changes to our diffing system
				last_game_info = current_request_game_info;
				last_map_name = current_request_map_name;
				last_steamlobby_id = current_request_steamlobby_id;
				last_player_count = current_request_player_count;
			}
			break;
		}
		case req_manual_update: {
			if (error_msg) {
				*error_context_ptr = "manual server update error";
				*error_ptr = error_msg;
			}
			else {
				// success!!!! (nothing to do)
			}
			break;
		}
		case req_shutdown: {
			if (error_msg) {
				*error_context_ptr = "shutdown server request error";
				*error_ptr = error_msg;
			}
			else {
				// success!!!! clear session details so we can start another one
				hosted_session_id = 0;
				hosted_session_secret = 0;
				hosted_session_max_players = -1;
			}
			break;
		}
		case req_publish: {
			auto result = _last_request->GetResult();
			if (!error_msg) {
				// process response
				size_t pos = result.find('\\');
				if (pos == std::string::npos) {
					*error_context_ptr = "had error publishing session";
					*error_ptr = "Malformed response recieved, could not delimit recieved ID & secret";
				}
				else {
					std::string left = result.substr(0, pos);
					std::string right = result.substr(pos + 1);
					hosted_session_id = std::stoull(left, nullptr, 16);
					hosted_session_secret = std::stoull(right, nullptr, 16);
					*error_context_ptr = "Successfully published session !!!!";
					*error_ptr = "request returned and gave us our correctly formatted ID & secret";
				}
			}
			else {
				*error_context_ptr = "had error publishing session";
				*error_ptr = error_msg;
			}
			break;
		}
		}
		_last_request = nullptr;
		last_routine_request.release();
	}

	if (attempting_shutdown || !hosted_session_id || !hosted_session_secret || !steam_userid || !steam_lobbyid) {
		return;
	}
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(now - last_session_update).count() < 15){
		return;
	}


	last_session_update = now;
	uint64_t current_steamlobby_id = steam_lobbyid;
	GameSessionInfo current_game_info = GetSessionDetails();

	// open Json
	json j;
	j["id"] = std::to_string(hosted_session_id);
	j["secret"] = std::to_string(hosted_session_secret);
	if (current_game_info.gametype_category != last_game_info.gametype_category || current_game_info.type != last_game_info.type) {
		j["game_mode"] = PrintGametype(current_game_info.gametype_category, current_game_info.type);
	}
	if (current_game_info.gamevar_name != last_game_info.gamevar_name) {
		j["game_variant"] = current_game_info.gamevar_name;
	}
	if (current_game_info.mod_name != last_game_info.mod_name) {
		j["mod_name"] = current_game_info.mod_name;
	}
	if (current_game_info.game != last_game_info.game) {
		j["mod_game_engine"] = PrintHaloGame(current_game_info.game);
	}
	if (current_game_info.workshop_id != last_game_info.workshop_id) {
		j["mod_workshop_id"] = std::to_string(current_game_info.workshop_id);
	}
	int current_players = 0;
	PlayerList* player_list = GetPlayersList();
	if (player_list) {
		current_players = player_list->player_count;
	}
	if (current_players != last_player_count) {
		j["current_players"] = std::to_string(current_players);
	}
	string current_map_name = current_game_info.mapvar_name;
	if (current_map_name.empty()) {
		current_map_name = current_game_info.map_name;
	}
	if (current_map_name != last_game_info.mapvar_name) {
		j["map_variant"] = current_map_name;
	}

	if (current_steamlobby_id != last_steamlobby_id) {
		j["steam_lobbyid"] = current_steamlobby_id;
	}
	
	last_request_type = req_routine;
	last_routine_request = std::make_unique<HttpRequest>(server_update_url, j.dump());

	current_request_game_info = current_game_info;
	current_request_map_name = current_map_name;
	current_request_steamlobby_id = current_steamlobby_id;
	current_request_player_count = current_players;
}



void PublishSession(char* _session_name_buffer, char* _session_desc_buffer, int _max_players, const vector<uint64_t>& _selected_mods) {
	attempting_shutdown = false;
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


	json j;

	j["name"] = string(_session_name_buffer);
	j["description"] = string(_session_desc_buffer);
	j["max_players"] = std::to_string(hosted_session_max_players);
	j["mods"] = modStrings;

	j["game_mode"] = PrintGametype(last_game_info.gametype_category, last_game_info.type);
	j["game_variant"] = last_game_info.gamevar_name;
	j["mod_name"] = last_game_info.mod_name;
	j["mod_game_engine"] = PrintHaloGame(last_game_info.game);
	j["mod_workshop_id"] = std::to_string(last_game_info.workshop_id);

	last_player_count = 0;
	PlayerList* player_list = GetPlayersList();
	if (player_list) {
		last_player_count = player_list->player_count;
	}
	j["current_players"] = std::to_string(last_player_count);

	last_map_name = last_game_info.mapvar_name;
	if (last_map_name.empty()) {
		last_map_name = last_game_info.map_name;
	}
	j["map_variant"] = last_map_name;
	


	j["steam_lobbyid"] = last_steamlobby_id;
	j["steam_id"] = steam_userid;

	last_request_type = req_publish;
	last_routine_request = std::make_unique<HttpRequest>(server_publish_url, j.dump());
}
bool ManualSessionUpdate(char* _session_name_buffer, char* _session_desc_buffer, int _max_players, const vector<uint64_t>& _selected_mods) {
	if (!hosted_session_id || !hosted_session_secret || IsRequesting()) {
		return false;
	}
	// static stuff that we have to intentionally change
	hosted_session_max_players = _max_players;

	std::vector<std::string> modStrings;
	modStrings.reserve(_selected_mods.size());
	for (uint64_t id : _selected_mods) {
		modStrings.push_back(std::to_string(id));
	}
	

	json j;
	j["id"] = std::to_string(hosted_session_id);
	j["secret"] = std::to_string(hosted_session_secret);
	j["name"] = string(_session_name_buffer);
	j["description"] = string(_session_desc_buffer);
	j["max_players"] = std::to_string(hosted_session_max_players);
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
	j["id"] = std::to_string(hosted_session_id);
	j["secret"] = std::to_string(hosted_session_secret);

	// add it to the thingo list
	last_request_type = req_shutdown;
	last_routine_request = std::make_unique<HttpRequest>(server_shutdown_url, j.dump());
	return true;
}


