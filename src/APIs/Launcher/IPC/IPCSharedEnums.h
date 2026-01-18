#pragma once

enum halo_game {
	halo1 = 0,
	halo2 = 1,
	halo3 = 2,
	halo4 = 3,
	halo2a = 4,
	halo3odst = 5,
	reach = 6,
	game_NONE,
};
enum gamemode_type {
	GameCategory_CTF = 0,
	GameCategory_Slayer = 1,
	GameCategory_Oddball = 2,
	GameCategory_KOTH = 3,
	GameCategory_Juggernaut = 4,
	GameCategory_Infection = 5,
	GameCategory_Flood,
	GameCategory_Race,
	GameCategory_Extraction,
	GameCategory_Dominion,
	GameCategory_Regicide,
	GameCategory_Grifball,
	GameCategory_Ricochet,
	GameCategory_Sandbox = 13,
	GameCategory_VIP,
	GameCategory_Territories,
	GameCategory_Assault,
	GameCategory_Payback,
	GameCategory_Stockpile,
	GameCategory_Headhunter,
	GameCategory_Invasion,
	GameCategory_Firefight = 21, // supposed to be 20??
	GameCategory_ActionSack,
	GameCategory_Campaign,
	GameCategory_Playlist,
	GameCategory_SpartanOps,
	GameCategory_PreGameWarmUp,
	GameCategory_GunGame,
	GameCategory_NONE,
};
enum map_type {
	campaign = 0,
	multiplayer = 1,
	firefight = 2,
	map_type_NONE,
};
const char* PrintHaloGame(halo_game game) {
	switch (game) {
	case halo1: return "H1";
	case halo2: return "H2";
	case halo3: return "H3";
	case halo4: return "H4";
	case reach: return "REACH";
	case halo2a: return "H2A";
	case halo3odst: return "ODST";
	default: return "MAIN MENU";
	}
}
const char* PrintGametype(gamemode_type gametype, map_type fallback_type) {
	switch (gametype) {
	case GameCategory_CTF: return "CTF";
	case GameCategory_Slayer: return "Slayer";
	case GameCategory_Oddball: return "Oddball";
	case GameCategory_KOTH: return "KOTH";
	case GameCategory_Juggernaut: return "Juggernaut";
	case GameCategory_Infection: return "Infection";
	case GameCategory_Flood: return "Flood";
	case GameCategory_Race: return "Race";
	case GameCategory_Extraction: return "Extraction";
	case GameCategory_Dominion: return "Dominion";
	case GameCategory_Regicide: return "Regicide";
	case GameCategory_Grifball: return "Grifball";
	case GameCategory_Ricochet: return "Ricochet";
	case GameCategory_Sandbox: return "Forge";
	case GameCategory_VIP: return "VIP";
	case GameCategory_Territories: return "Territories";
	case GameCategory_Assault: return "Assault";
	case GameCategory_Payback: return "Payback";
	case GameCategory_Stockpile: return "Stockpile";
	case GameCategory_Headhunter: return "Headhunter";
	case GameCategory_Invasion: return "Invasion";
	case GameCategory_Firefight: return "Firefight";
	case GameCategory_ActionSack: return "Action Sack";
	case GameCategory_Campaign: return "Campaign";
	case GameCategory_Playlist: return "Playlist";
	case GameCategory_SpartanOps: return "Spartan Ops";
	case GameCategory_PreGameWarmUp: return "Pre-game";
	case GameCategory_GunGame: return "Gun Game";
	default:
		switch (fallback_type) {
		case campaign: return "UNK Campaign"; // should be impossible to get this
		case multiplayer: return "UNK multiplayer";
		case firefight: return "UNK Firefight";
		default: return "MAIN MENU";
		}
	}
}