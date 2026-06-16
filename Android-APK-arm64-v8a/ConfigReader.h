#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cctype>

class ConfigReader {
public:
    bool load(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == ';') continue;

            size_t eqPos = line.find('=');
            if (eqPos == std::string::npos) continue;

            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));

            if (!key.empty()) {
                m_data[key] = value;
            }
        }
        return true;
    }

    std::string get(const std::string& key, const std::string& defaultValue = "") const {
        auto it = m_data.find(key);
        return (it != m_data.end()) ? it->second : defaultValue;
    }

    int getInt(const std::string& key, int defaultValue = 0) const {
        std::string val = get(key, "");
        if (val.empty()) return defaultValue;
        try {
            return std::stoi(val);
        }
        catch (...) {
            return defaultValue;
        }
    }

    bool getBool(const std::string& key, bool defaultValue = false) const {
        std::string val = get(key, "");
        if (val.empty()) return defaultValue;
        if (val == "true" || val == "1" || val == "yes" || val == "on") return true;
        if (val == "false" || val == "0" || val == "no" || val == "off") return false;
        return defaultValue;
    }

    bool has(const std::string& key) const {
        return m_data.find(key) != m_data.end();
    }

private:
    std::unordered_map<std::string, std::string> m_data;

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r\f\v");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\n\r\f\v");
        return s.substr(start, end - start + 1);
    }
};