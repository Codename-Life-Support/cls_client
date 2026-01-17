#pragma once
#include "../../res/steam/include/steam_api.h"

#include <fstream>
#include <iostream>
using json = nlohmann::json;



static HANDLE steam_thread = INVALID_HANDLE_VALUE;
static bool running = true;

// TODO: make thread safe
uint64_t steam_userid = 0;
uint64_t steam_lobbyid = 0;
string GetJoinLink() {
	return "steam://joinlobby/976730/" + to_string(steam_lobbyid) + "/" + to_string(steam_userid);
}


DWORD WINAPI SteamLoop(LPVOID lpParam){
	while (!SteamAPI_Init) Sleep(1000);
	ISteamFriends* steam_friends;
	ISteamUser* steam_user;
	while (running){
		Sleep(1000);
		steam_friends = SteamFriends();
		steam_user = SteamUser();
		if (!steam_friends || !steam_user) continue;
		if (!steam_user->BLoggedOn()) continue;

		CSteamID uid = steam_user->GetSteamID();
		FriendGameInfo_t friend_game_info = {};
		if (!steam_friends->GetFriendGamePlayed(uid, &friend_game_info))
			continue;


		// then update details, and leave a notify for the poller that details have changed!!
		steam_userid = uid.ConvertToUint64();
		steam_lobbyid = friend_game_info.m_steamIDLobby.ConvertToUint64();
	}
	return 0;
}




vector<WorkshopMod> Steam_GetInstalledMods() {
	vector<WorkshopMod> mod_list = {};

	ISteamUGC* steam_ugc = SteamUGC();

	PublishedFileId_t mod_ids[1000] = {};
	uint32_t installed_mods = steam_ugc->GetSubscribedItems(mod_ids, 1000);
	if (!installed_mods){
		return mod_list;
	}

	char filepath_buffer[512];
	filepath_buffer[0] = '\0';
	for (int i = 0; i < installed_mods; i++) {
		uint64_t mod_size = 0;
		uint32_t mod_timestamp = 0;
		if (steam_ugc->GetItemInstallInfo(mod_ids[i], &mod_size, filepath_buffer, 512, &mod_timestamp))
		{
			ParseUGCIntoModIds(mod_list, filepath_buffer, mod_ids[i]);
		}
	}

	return mod_list;
}

unordered_map<GUID, ModMap, GuidHasher, GuidEqual> Steam_CacheModdedMaps() {
	unordered_map<GUID, ModMap, GuidHasher, GuidEqual> mod_list = {};

	ISteamUGC* steam_ugc = SteamUGC();

	PublishedFileId_t mod_ids[1000] = {};
	uint32_t installed_mods = steam_ugc->GetSubscribedItems(mod_ids, 1000);
	if (!installed_mods) {
		return mod_list;
	}

	char filepath_buffer[512];
	filepath_buffer[0] = '\0';
	for (int i = 0; i < installed_mods; i++) {
		uint64_t mod_size = 0;
		uint32_t mod_timestamp = 0;
		if (steam_ugc->GetItemInstallInfo(mod_ids[i], &mod_size, filepath_buffer, 512, &mod_timestamp))
		{
			ParseUGCIntoMapGuids(mod_list, filepath_buffer, mod_ids[i]);
		}
	}

	return mod_list;
}



void InjectedSteamInit(){
	running = true;
	steam_thread = CreateThread(nullptr,0, SteamLoop,nullptr,0,nullptr);
}
void InjectedSteamClose(){
	running = false;
	WaitForSingleObject(steam_thread, INFINITE);
}
