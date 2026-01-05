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
    char Guid[17];     // 0xB3
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
uint64_t evil_players = 0;
PlayerList* player_list = nullptr;
#pragma pack(pop)
