#pragma once


const int SHARED_BUFFER_SIZE = 512;

#pragma pack(push, 1)
struct to_launcher_struct {
    uint64_t steam_lobbyid;
    uint64_t steam_hostid;
    int32_t players;
    int32_t max_players; // -1 unless we're hosting
    uint64_t hosted_lobbyid; // 0 unless hosting
    uint32_t focus_launcher; // lowest bit is used to tell whether our request wants CGB focus or not
};

struct to_overlay_struct {
    char lobby_name[64];
    char lobby_summary[256];
    uint64_t steam_lobbyid;
    uint64_t auto_join_timestamp; // 0 means null, 1 means instant, other means timestamp that we'll be joined into a session
};
#pragma pack(pop)
