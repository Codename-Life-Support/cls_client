#pragma once



unordered_map<GUID, ModMap, GuidHasher, GuidEqual> cached_mod_lookup = {};

char** session_struct_ptr_ptr = 0;
char**** global_mapinfo_ptr = 0;

typedef uint64_t(GetMapInfoPtr)(void* global_mapinfo_obj, void* map_guid);
GetMapInfoPtr* GetMapInfo = 0;


typedef uint64_t(ObjThisCall)(void* _this);
typedef uint64_t(ObjThisCallRDX)(void* _this, void* RDX);
typedef uint64_t(ObjThisCallRDXR8R9)(void* _this, void* RDX, void* R8, void* R9);


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
		case campaign: return "UNK Campaign";
		case multiplayer: return "UNK multiplayer";
		case firefight: return "UNK Firefight";
		default: return "MAIN MENU";
		}
	}
}

// all get names of this vtable require an RDX param, which points to a 16 struct for the string info to pasted into
#pragma pack(push, 1)
struct mapinfo_func_table {
	void* destructor;  // 0x00
	void* GetHaloGame; // 0x08
	void* GetGuidPtr; // 0x10
	ObjThisCall* GetMapType; // 0x18
	ObjThisCallRDX* GetMapImageUnkName; // sometimes nullptr // 0x20
	ObjThisCallRDX* GetDefaultMapName; // 0x28
	ObjThisCallRDX* GetDefaultMapDesc; // 0x30
	ObjThisCallRDX* GetMapImageName; // 0x38
	ObjThisCallRDX* GetMapLargeImageName; // 0x40
	ObjThisCallRDX* GetMapImageLoadingName; // 0x48;
	ObjThisCallRDXR8R9* GetMapImageMiniName; // 0x50 // take R8, R9, must be valid int ptrs
	//void* unk_0x58; // access RCX + 40 <- crash
	void* unk_0x60;  // gets heap ptr : 0, in region that contains map file names?
	void* GetMapInsertionPoint; // 0x68
	ObjThisCallRDX* GetInsertionPointName; // 0x70 // i think thats what this does at least
};
struct mapvar_func_table {
	void* destructor;
	// 0x08
	ObjThisCall* GetMapName;
	void* unk_0x10;
	void* unk_0x18;
	void* unk_0x20;
	void* unk_0x28;
	void* unk_0x30;
	void* unk_0x38;
	void* unk_0x40;
	void* unk_0x48; // access something
	void* unk_0x50; // access something
	void* unk_0x58;
	void* unk_0x60;
	// 0x68
	void* GetIsPregame;
};
struct gametype_func_table {
	void* destructor;
	ObjThisCall* GetModeName; // 0x08
	void* SetModeName; // 0x10
	ObjThisCall* GetModeDesc; // 0x18
	void* SetModeDesc; // 0x20
	void* unk_0x28; // access something: 1
	void* unk_0x30; // access something: 1
	void* unk_0x38;
	void* unk_0x40;
	void* unk_0x48; 
	void* GetCategoryName; // 0x50
	ObjThisCall* GetCategory; // 0x58
	void* GetIsForge; // 0x60 // (byte) 
	void* GetIsPregame; // check pregame? : 0
	void* unk_0x70; // access something : 1 ?? teams enabled??
	// 0x78 : gets 0xFF01
	// 0x80 : gets 0x8
	// 0x88 : gets ptr to self??
	// 0x90 : gets 0
	// 0x98 : crashes
};
#pragma pack(pop)


struct GameSessionInfo {
	halo_game game;

	map_type type;
	string map_name;

	string mod_name;
	uint64_t workshop_id; // matches up mod guid with installed mods to find this
	
	string mapvar_name;
	string gamevar_name;
	gamemode_type gametype_category;
};

GameSessionInfo GetSessionDetails() {
	GameSessionInfo details;
	details.game = game_NONE;
	details.type = map_type_NONE;
	details.map_name = "";
	details.mod_name = "";
	details.workshop_id = 0;
	details.mapvar_name = "";
	details.gamevar_name = "";
	details.gametype_category = GameCategory_NONE;
	if (!GameState_GetIsIngame()) {
		MessageBoxA(0, "out of game report", "report status ", MB_OK);
		return details;
	}

	if (!session_struct_ptr_ptr || !*session_struct_ptr_ptr || !GetMapInfo || !global_mapinfo_ptr) {
		return details;
	}
	char** test1 = *(*global_mapinfo_ptr + 4); // +0x20
	if (!test1) {
		return details;
	}
	char* test2 = *(test1 + 17); // +0x88
	if (!test2) {
		return details;
	}
	
	char* session_struct_ptr = *session_struct_ptr_ptr;
	mapinfo_func_table** mapinfo_struct = (mapinfo_func_table**)GetMapInfo(test2, session_struct_ptr + 0x7C);
	if (!mapinfo_struct) {
		return details;
	}

	MessageBoxA(0, "in game report", "report status ", MB_OK);

	mapvar_func_table** mapvar_struct = *(mapvar_func_table***)(session_struct_ptr + 0x2C080);
	gametype_func_table** gametype_struct = *(gametype_func_table***)(session_struct_ptr + 0x2C088);

	char output_buffer[16];

	details.game = (halo_game)*(int*)session_struct_ptr;

	details.type = (map_type)(uint8_t)((*mapinfo_struct)->GetMapType)(mapinfo_struct);

	char** mapname_ptr = (char**)((*mapinfo_struct)->GetDefaultMapName)(mapinfo_struct, output_buffer);
	if (mapname_ptr && *mapname_ptr) {
		details.map_name = *mapname_ptr;
	}

	// find workshop ID & mod name based off of GUID
	int mapid = *(int*)(session_struct_ptr + 0x78);

	//char buffer[32];
	//sprintf_s(buffer, "%d", mapid);
	//MessageBoxA(nullptr, buffer, "mapid", MB_OK);

	//sprintf_s(buffer, "%d", details.type);
	//MessageBoxA(nullptr, buffer, "type id ", MB_OK);



	if (mapid == -3) {
		//MessageBoxA(0, "playing a mod detected!!!", "GameSession failure", MB_OK);
		GUID current_guid = *(GUID*)(session_struct_ptr + 0x7C);

		bool has_recached = false;
		while (true) {
			auto it = cached_mod_lookup.find(current_guid);
			if (it != cached_mod_lookup.end()) {
				ModMap& mod = it->second;
				details.workshop_id = mod.workshop_id;
				details.mod_name = mod.workshop_name;
				break;
			}
			else if (!has_recached)
			{
				// try recaching, as the installed mods might have changed?
				cached_mod_lookup = Steam_CacheModdedMaps();
				has_recached = true;
			} 
			else 
			{
				MessageBoxA(0, "Failed to get current mod after recaching!!! this should be impossible..", "GameSession failure", MB_OK);
				break;
			}
		}

	}

	// get gamevar info
	if (mapvar_struct) {
		wchar_t* mapvarname_ptr = (wchar_t*)((*mapvar_struct)->GetMapName)(mapvar_struct);
		if (mapvarname_ptr) {
			details.mapvar_name = wchar_to_utf8(mapvarname_ptr);
		}
	}
	if (gametype_struct) {
		details.gametype_category = (gamemode_type)((*gametype_struct)->GetCategory)(gametype_struct);
		wchar_t* modename_ptr = (wchar_t*)((*gametype_struct)->GetModeName)(gametype_struct);
		if (modename_ptr) {
			details.gamevar_name = wchar_to_utf8(modename_ptr);
		}
	}

	return details;
}

struct session_s_epic {
	// 0x00
	int halo_game;
	// 0x50
	int also_halo_game;

	// 0x78
	int map_id; // this is the map index?
	// 0x7C
	uint64_t map_guid_1; // either a 4byte in or a 16 byte UGC GUID
	uint64_t map_guid_2;

	// 0x2C080
	mapvar_func_table** mapvar;
	// 0x2C088
	gametype_func_table** gametype;
};