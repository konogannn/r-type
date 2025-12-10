/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** PathHelper
*/

#include "PathHelper.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

namespace rtype {
namespace utils {

std::string PathHelper::_basePath = "";
bool PathHelper::_initialized = false;

void PathHelper::initialize()
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
    std::cerr << "Warning: Assets directory not found. Using current directory."
              << std::endl;
    std::cerr << "Current working directory: "
              << std::filesystem::current_path().string() << std::endl;
}

std::string PathHelper::getAssetPath(const std::string& path)
{
    initialize();

    if (std::filesystem::path(path).is_absolute()) {
        return path;
    }

    std::filesystem::path fullPath = std::filesystem::path(_basePath) / path;

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

void PathHelper::reset()
{
    _initialized = false;
    _basePath = "";
}

}  // namespace utils
}  // namespace rtype
