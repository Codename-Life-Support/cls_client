#pragma once



void WriteSessionValues(const std::string& left, const std::string& right)
{
    std::ofstream out(std::filesystem::current_path() / "session_info.txt", std::ios::trunc);
    if (!out)
        return; // or handle error explicitly

    out << "hosted_session_id=" << left << "\n";
    out << "hosted_session_secret=" << right << "\n";
}
bool ReadSessionValues(string& hosted_session_id, string& hosted_session_secret)
{
    std::ifstream in(std::filesystem::current_path() / "session_info.txt");
    if (!in)
        return false;

    std::string line;

    int found = 0;
    while (std::getline(in, line)) {
        // Trim whitespace if needed — but assuming clean file format
        if (line.rfind("hosted_session_id=", 0) == 0) {
            std::string value = line.substr(strlen("hosted_session_id="));
            hosted_session_id = value;
            found += 1;
        }
        else if (line.rfind("hosted_session_secret=", 0) == 0) {
            std::string value = line.substr(strlen("hosted_session_secret="));
            hosted_session_secret = value;
            found += 1;
        }
    }
    return (found == 2);
}
