/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Config
*/

#pragma once

#include <map>
#include <string>

namespace rtype {

/**
 * @brief Simple configuration manager with JSON file persistence
 */
class Config {
   public:
    /**
     * @brief Get the singleton instance
     */
    static Config& getInstance();

    /**
     * @brief Load configuration from file
     * @param filepath Path to config file (default: "config.json")
     * @return true if loaded successfully
     */
    bool load(const std::string& filepath = "config.json");

    /**
     * @brief Save configuration to file
     * @param filepath Path to config file (default: "config.json")
     * @return true if saved successfully
     */
    bool save(const std::string& filepath = "config.json");

    /**
     * @brief Get a float value
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Value or default
     */
    float getFloat(const std::string& key, float defaultValue = 0.0f);

    /**
     * @brief Set a float value
     * @param key Configuration key
     * @param value Value to set
     */
    void setFloat(const std::string& key, float value);

    /**
     * @brief Get an int value
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Value or default
     */
    int getInt(const std::string& key, int defaultValue = 0);

    /**
     * @brief Set an int value
     * @param key Configuration key
     * @param value Value to set
     */
    void setInt(const std::string& key, int value);

    /**
     * @brief Get a string value
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Value or default
     */
    std::string getString(const std::string& key,
                          const std::string& defaultValue = "");

    /**
     * @brief Set a string value
     * @param key Configuration key
     * @param value Value to set
     */
    void setString(const std::string& key, const std::string& value);

   private:
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::map<std::string, std::string> _data;
};

}  // namespace rtype
