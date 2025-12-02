/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Config
*/

#include "Config.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace rtype {

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Config: Could not open " << filepath << ", using defaults"
                  << std::endl;
        setFloat("musicVolume", 80.0f);
        setFloat("sfxVolume", 100.0f);
        return false;
    }

    _data.clear();
    std::string line;
    while (std::getline(file, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        auto removeChars = [](std::string& str, const std::string& chars) {
            for (char c : chars) {
                str.erase(std::remove(str.begin(), str.end(), c), str.end());
            }
        };

        removeChars(key, " \t\n\r\"{}");
        removeChars(value, " \t\n\r\",{}");

        if (!key.empty() && !value.empty()) {
            _data[key] = value;
        }
    }

    file.close();
    return true;
}

bool Config::save(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Config: Could not save to " << filepath << std::endl;
        return false;
    }

    file << "{\n";
    size_t count = 0;
    for (const auto& [key, value] : _data) {
        file << "  \"" << key << "\": ";

        bool isNumber = false;
        if (!value.empty()) {
            try {
                size_t pos;
                std::stod(value, &pos);
                isNumber = (pos == value.length());
            } catch (...) {
                isNumber = false;
            }
        }

        if (isNumber) {
            file << value;
        } else {
            file << "\"" << value << "\"";
        }

        if (++count < _data.size()) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";

    file.close();
    return true;
}

float Config::getFloat(const std::string& key, float defaultValue) {
    auto it = _data.find(key);
    if (it == _data.end()) {
        return defaultValue;
    }
    try {
        return std::stof(it->second);
    } catch (...) {
        return defaultValue;
    }
}

void Config::setFloat(const std::string& key, float value) {
    _data[key] = std::to_string(value);
}

int Config::getInt(const std::string& key, int defaultValue) {
    auto it = _data.find(key);
    if (it == _data.end()) {
        return defaultValue;
    }
    try {
        return std::stoi(it->second);
    } catch (...) {
        return defaultValue;
    }
}

void Config::setInt(const std::string& key, int value) {
    _data[key] = std::to_string(value);
}

std::string Config::getString(const std::string& key,
                              const std::string& defaultValue) {
    auto it = _data.find(key);
    if (it == _data.end()) {
        return defaultValue;
    }
    return it->second;
}

void Config::setString(const std::string& key, const std::string& value) {
    _data[key] = value;
}

}  // namespace rtype
