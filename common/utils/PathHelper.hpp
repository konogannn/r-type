/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** PathHelper - Cross-platform path utilities
*/

#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace rtype {
namespace utils {

/**
 * @brief Helper class for cross-platform path management
 *
 * This class ensures asset paths work correctly on both Windows and Unix-like
 * systems, regardless of the working directory when the executable is launched.
 */
class PathHelper {
   private:
    static std::string _basePath;
    static bool _initialized;

    /**
     * @brief Initialize the base path by searching for the assets directory
     */
    static void initialize()
    {
        if (_initialized) return;

        std::vector<std::string> possiblePaths = {
            "",           // Current directory
            "./",         // Current directory (explicit)
            "../",        // Parent directory
            "../../",     // Two levels up
            "../../../",  // Three levels up (for build subdirectories)
        };

        for (const auto& basePath : possiblePaths) {
            std::filesystem::path testPath =
                std::filesystem::path(basePath) / "assets";
            if (std::filesystem::exists(testPath) &&
                std::filesystem::is_directory(testPath)) {
                _basePath = basePath;
                _initialized = true;
                return;
            }
        }

        _basePath = "";
        _initialized = true;
        std::cerr
            << "Warning: Assets directory not found. Using current directory."
            << std::endl;
        std::cerr << "Current working directory: "
                  << std::filesystem::current_path().string() << std::endl;
    }

   public:
    /**
     * @brief Get the full path for an asset file
     *
     * Searches for the asset in multiple possible locations to handle
     * different working directories on Windows and Linux.
     *
     * @param path The asset path (e.g., "assets/sprites/player.png")
     * @return std::string The full path to the asset
     *
     * @example
     * auto path = PathHelper::getAssetPath("assets/sprites/player.png");
     * sprite->loadTexture(path);
     */
    static std::string getAssetPath(const std::string& path)
    {
        initialize();

        if (std::filesystem::path(path).is_absolute()) {
            return path;
        }

        std::filesystem::path fullPath =
            std::filesystem::path(_basePath) / path;

        if (std::filesystem::exists(fullPath)) {
            return fullPath.string();
        }

        std::cerr << "Warning: Asset file not found: " << fullPath.string()
                  << std::endl;
        std::cerr << "  Searched path: " << path << std::endl;
        std::cerr << "  Base path: " << _basePath << std::endl;
        std::cerr << "  Working directory: "
                  << std::filesystem::current_path().string() << std::endl;

        return fullPath.string();
    }

    /**
     * @brief Reset the path helper (mainly for testing)
     */
    static void reset()
    {
        _initialized = false;
        _basePath = "";
    }
};

}  // namespace utils
}  // namespace rtype
