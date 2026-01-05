#pragma once
#include "../../res/steam/include/steam_api.h"



static HANDLE steam_thread = INVALID_HANDLE_VALUE;
static bool running = true;


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

		steam_userid = uid.ConvertToUint64();
		steam_lobbyid = friend_game_info.m_steamIDLobby.ConvertToUint64();
		//MessageBoxA(0, "testing", "DirectX hook failure", 0);
	}
	return 0;
}

void InjectedSteamInit(){
	running = true; // redundant
	steam_thread = CreateThread(nullptr,0, SteamLoop,nullptr,0,nullptr);
}
void InjectedSteamClose(){
	running = false;
	WaitForSingleObject(steam_thread, INFINITE);
}
