#pragma once



struct overlay_log {
	string context;
	string message;
	bool is_error;
	int index;
};

int current_log_index = 0;
vector<overlay_log> logs = {};
std::mutex log_mutex;

void PushLog(string context, string message, bool is_error = true) {
	overlay_log new_log;
	new_log.context = context;
	new_log.message = message;
	new_log.is_error = is_error;

	std::lock_guard<std::mutex> lock(log_mutex);
	new_log.index = logs.size();
	logs.push_back(new_log);
}

int GetLogCount() {
	std::lock_guard<std::mutex> lock(log_mutex);
	return logs.size();
}

bool GetCurrentLog(overlay_log& output) {
	std::lock_guard<std::mutex> lock(log_mutex);
	if (current_log_index >= logs.size()) {
		return false;
	}

	output = logs[current_log_index];
	return true;
}
void NextLog() {
	std::lock_guard<std::mutex> lock(log_mutex);
	if (current_log_index < logs.size()) {
		current_log_index += 1;
	}
}

void DumpLogs() {

}

