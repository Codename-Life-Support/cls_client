#pragma once

uint32_t* SquadPrivacy;

uint8_t* IsInMainMenu;
uint8_t* IsInGame1;
uint8_t* IsInMenu;
uint8_t* IsPaused;
uint8_t* IsLoading;
uint8_t* IsPauseMenu;
uint8_t* IsEndingGame;
uint8_t* IsLeavingGame;
uint8_t* IsLoadingBackToMenu;
uint8_t* IsInGame2;
uint8_t* IsInGame3;

// TODO: decide on a SINGLE bool to use
bool GameState_GetIsIngame() {
	return ((*IsInGame1 || *IsInGame2 || *IsInGame3) && !*IsLoadingBackToMenu);
}
bool GameState_IsInSomeMenu() {
	return ((!*IsInGame1 || !*IsInGame2 || !*IsInGame3) || *IsPauseMenu || *IsLoading);
}

bool GameState_EnforceOpenPrivacy() {
	if (*SquadPrivacy == 2) {
		*SquadPrivacy = 3;
		return true;
	}
	return false;
}

void GameState_Hooks(uint64_t base_ptr) {
	SquadPrivacy = (uint32_t*)(base_ptr + 0x3F927FC);

	IsInMainMenu		= (uint8_t*)(base_ptr + 0x4000B18);
	IsInGame1			= (uint8_t*)(base_ptr + 0x4000B60);
	IsInMenu			= (uint8_t*)(base_ptr + 0x4000B8A);
	IsPaused			= (uint8_t*)(base_ptr + 0x4000B8B);
	IsLoading			= (uint8_t*)(base_ptr + 0x4000B8C);
	IsPauseMenu			= (uint8_t*)(base_ptr + 0x4000B97);
	IsEndingGame		= (uint8_t*)(base_ptr + 0x4000B99);
	IsLeavingGame		= (uint8_t*)(base_ptr + 0x4000B9A);
	IsLoadingBackToMenu = (uint8_t*)(base_ptr + 0x4000B9C);
	IsInGame2			= (uint8_t*)(base_ptr + 0x4000B9D);
	IsInGame3			= (uint8_t*)(base_ptr + 0x4000B9F);
}