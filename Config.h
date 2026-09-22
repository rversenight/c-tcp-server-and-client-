#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <iostream>

struct OuchConfig {
    bool enabled = false;
    std::string desc;
    std::string client_account;
    std::string account_number;
    std::string username;
    std::string pass;
    std::string ip;
    int port = 0;
    int client_category = 1;
};

class ConfigLoader {
private:
    static std::string extract_value(const std::string& line) {
        size_t colon = line.find(':');
        if (colon == std::string::npos) return "";
        std::string val = line.substr(colon + 1);
        val.erase(0, val.find_first_not_of(" \t\r\n\""));
        size_t last = val.find_last_not_of(" \t\r\n\",");
        if (last != std::string::npos) val = val.substr(0, last + 1);
        return val;
    }

public:
    static bool load(const std::string& filename, OuchConfig& config) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        while (std::getline(file, line)) {
            if (line.find("\"enabled\"") != std::string::npos) config.enabled = (line.find("true") != std::string::npos);
            else if (line.find("\"desc\"") != std::string::npos) config.desc = extract_value(line);
            else if (line.find("\"client_account\"") != std::string::npos) config.client_account = extract_value(line);
            else if (line.find("\"account_number\"") != std::string::npos) config.account_number = extract_value(line);
            else if (line.find("\"username\"") != std::string::npos) config.username = extract_value(line);
            else if (line.find("\"pass\"") != std::string::npos) config.pass = extract_value(line);
            else if (line.find("\"ip\"") != std::string::npos) config.ip = extract_value(line);
            else if (line.find("\"port\"") != std::string::npos) {
                std::string p = extract_value(line);
                if (!p.empty()) config.port = std::stoi(p);
            } else if (line.find("\"client_category\"") != std::string::npos) {
                std::string c = extract_value(line);
                if (!c.empty()) config.client_category = std::stoi(c);
            }
        }
        file.close();
        return true;
    }
};

#endif
