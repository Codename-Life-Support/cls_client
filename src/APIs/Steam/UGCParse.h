#pragma once
using json = nlohmann::json;


enum ModEngine {
	Halo1,
	Halo2,
	Halo3,
	Halo4,
	Halo2A,
	ODST,
	Reach,
	ErrorType
};
ModEngine ParseEngine(string engine) {
	if (engine == "Halo1") {
		return Halo1;
	}
	else if (engine == "Halo2") {
		return Halo2;
	}
	else if (engine == "Halo3") {
		return Halo3;
	}
	else if (engine == "Halo4") {
		return Halo4;
	}
	else if (engine == "Halo2A") {
		return Halo2A;
	}
	else if (engine == "Halo3ODST") {
		return ODST;
	}
	else if (engine == "HaloReach") {
		return Reach;
	}
	return ErrorType;
}
const char* UnparseEngine(ModEngine engine) {
	switch (engine) {
	case Halo1:  return "H1";
	case Halo2:  return "H2";
	case Halo3:  return "H3";
	case Halo4:  return "H4";
	case Halo2A: return "H2A";
	case ODST:   return "ODST";
	case Reach:  return "REACH";
	default:     return "UNK";
	}
}
struct ModMap {
	string name;
	string workshop_name;
	uint64_t workshop_id;
};
struct WorkshopMod {
	string name;
	ModEngine engine;
	uint64_t workshop_id;
};


bool LoadJson(const std::string& path, json& output_json)
{
	if (!filesystem::exists(path)) {
		return false;
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		return false;
	}

	try
	{
		file >> output_json;
		return true;
	}
	catch (const std::exception& e)
	{
		return false;
	}
}


#include <rpc.h>
#pragma comment(lib, "Rpcrt4.lib")

GUID StringToGuid(const std::string& str)
{
	GUID guid{};
	RPC_STATUS status = UuidFromStringA((RPC_CSTR)str.c_str(), &guid);

	if (status != RPC_S_OK) {
		throw std::runtime_error("Invalid GUID string: " + str);
	}
	return guid;
}


bool ParseMapJson(const string& filepath, string& output_mapname, GUID& output_guid) {
	json loaded_json;
	if (!LoadJson(filepath, loaded_json))
	{
		MessageBoxA(0, "Failed to load a mod json", "Mod check failure", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	const auto& mod_title_wrapper = loaded_json["Title"];
	output_mapname = mod_title_wrapper["Neutral"];
	string map_guid = loaded_json["MapGuid"];
	output_guid = StringToGuid(map_guid);
	return true;
}






void ParseUGCIntoModIds(vector<WorkshopMod>& names, const char* filepath_buffer, uint64_t workshopid) {

	string filepath = string(filepath_buffer) + "/";

	json loaded_json;
	if (!LoadJson(filepath + "ModInfo.json", loaded_json))
	{
		MessageBoxA(0, "Failed to load a mod json", "Mod check failure", MB_OK | MB_ICONINFORMATION);
		return;
	}

	const auto& workshop_title_wrapper = loaded_json["Title"];
	std::string workshop_title = workshop_title_wrapper["Neutral"];

	ModEngine engine = ParseEngine(loaded_json["Engine"]);

	names.push_back(WorkshopMod{ workshop_title, engine, workshopid });
}
void ParseUGCIntoMapGuids(unordered_map<GUID, ModMap, GuidHasher, GuidEqual>& dump, const char* filepath_buffer, uint64_t workshopid) {
	string filepath = string(filepath_buffer) + "/";

	json loaded_json;
	if (!LoadJson(filepath + "ModInfo.json", loaded_json))
	{
		MessageBoxA(0, "Failed to load a mod json", "Mod check failure", MB_OK | MB_ICONINFORMATION);
		return;
	}

	const auto& workshop_title_wrapper = loaded_json["Title"];
	std::string workshop_title = workshop_title_wrapper["Neutral"];

	const auto& content_wrapper = loaded_json["GameModContents"];
	if (content_wrapper.contains("MultiplayerMaps") && content_wrapper["MultiplayerMaps"].is_array()) {
		for (std::string item : content_wrapper["MultiplayerMaps"])
		{
			string mapname;
			GUID mapguid;
			if (ParseMapJson(filepath + item, mapname, mapguid)) {
				dump[mapguid] = ModMap{ mapname, workshop_title, workshopid };
			}
		}
	}
	if (content_wrapper.contains("FirefightMaps") && content_wrapper["FirefightMaps"].is_array()) {
		for (std::string item : content_wrapper["FirefightMaps"])
		{
			string mapname;
			GUID mapguid;
			if (ParseMapJson(filepath + item, mapname, mapguid)) {
				dump[mapguid] = ModMap{ mapname, workshop_title, workshopid };
			}
		}
	}

	// check for optional campaign maps json
	if (LoadJson(filepath + "CampaignInfo.json", loaded_json))
	{
		if (loaded_json.contains("CampaignMaps") && loaded_json["CampaignMaps"].is_array()) {
			for (std::string item : loaded_json["CampaignMaps"])
			{
				string mapname;
				GUID mapguid;
				if (ParseMapJson(filepath + item, mapname, mapguid)) {
					dump[mapguid] = ModMap{ mapname, workshop_title, workshopid };
				}
			}
		}

	}



}
