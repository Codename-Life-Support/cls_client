#pragma once



#pragma pack(push, 1)
struct PlayerInfo{
    uint32_t state;          // 0x00
    uint32_t flags0_0;       // 0x04
    uint32_t flags0_1;       // 0x08
    uint32_t unk0C;          // 0x0C
    uint64_t unk10;          // 0x10
    uint64_t unk18;          // 0x18
    wchar_t gamertag0[16];   // 0x20
    uint64_t XUID;           // 0x40
    char unk48;              // 0x48 (12)
    char unk49;              // 0x49 (-1)
    uint16_t unk4A;          // 0x4A -- PADDING
    uint32_t unk4C;          // 0x4C -- PADDING
    uint64_t networkId;      // 0x50
    uint64_t unk58;          // 0x58 (2)
    uint64_t unk60;          // 0x60
    uint32_t unk68[7];       // 0x68 -> 0x84
    wchar_t serviceId[4];    // 0x84
    wchar_t clanTag[16];     // 0x8C
    unsigned char flags[7];  // 0xAC
    char guid[17];           // 0xB3
    uint32_t unkC4;          // 0xC4
    uint64_t unkC8;          // 0xC8
    wchar_t gamertag1[16];   // 0xD0
    char unkF0[32];          // 0xF0
};
struct PlayerList {
    uint64_t player_count;
    uint64_t player_bitmask; // bits are checked to indicate which indicies are valid
    PlayerInfo players[16];
};
#pragma pack(pop)

int last_start_game_ack = 0;
int start_game_ack_number = 0;
uint64_t evil_players = 0;
void** lobby_info = nullptr;

bool GetStartAck(uint64_t* players_mask) {
    if (!players_mask) return false;

    if (last_start_game_ack != start_game_ack_number) {
        last_start_game_ack = start_game_ack_number;
        *players_mask = evil_players;
        return true;
    }
    return false;
}


PlayerList* GetPlayersList() {
    // not sure why it can be -0x30, MCCs code just has a check for this
    if (lobby_info != 0 && *lobby_info != 0 && (long)*lobby_info != -0x30) {
        return (PlayerList*)(((char*)*lobby_info) + 0x1D18);
    }
    return 0;
}


typedef HRESULT(player_content_check)();
extern "C" player_content_check* player_content_check_ptr;
extern "C" void ReportPlayersHaveContent(uint32_t* bitmask_ptr);
extern "C" void AsmDetour0();
player_content_check* player_content_check_ptr = NULL;
void ReportPlayersHaveContent(uint32_t* bitmask_ptr) {
    // TODO: make thread safe
    evil_players = *bitmask_ptr;
    start_game_ack_number = start_game_ack_number + 1;
}
