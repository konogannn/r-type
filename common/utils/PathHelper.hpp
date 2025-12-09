/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** PathHelper
*/

#pragma once

#include <string>

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
    static void initialize();

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
    static std::string getAssetPath(const std::string& path);

    /**
     * @brief Reset the path helper (mainly for testing)
     */
    static void reset();
};

}  // namespace utils
}  // namespace rtype
