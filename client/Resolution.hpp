/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** Resolution
*/

#pragma once

namespace rtype {

/**
 * @brief Available screen resolutions in 16:9 format
 */
enum class Resolution {
    R1280x720,   // HD Ready (720p)
    R1600x900,   // HD+
    R1920x1080,  // Full HD (1080p)
    R2560x1440   // QHD (1440p)
};

/**
 * @brief Get width for a resolution
 */
inline int getResolutionWidth(Resolution res) {
    switch (res) {
        case Resolution::R1280x720:
            return 1280;
        case Resolution::R1600x900:
            return 1600;
        case Resolution::R1920x1080:
            return 1920;
        case Resolution::R2560x1440:
            return 2560;
        default:
            return 1920;
    }
}

/**
 * @brief Get height for a resolution
 */
inline int getResolutionHeight(Resolution res) {
    switch (res) {
        case Resolution::R1280x720:
            return 720;
        case Resolution::R1600x900:
            return 900;
        case Resolution::R1920x1080:
            return 1080;
        case Resolution::R2560x1440:
            return 1440;
        default:
            return 1080;
    }
}

/**
 * @brief Get display name for a resolution
 */
inline const char* getResolutionName(Resolution res) {
    switch (res) {
        case Resolution::R1280x720:
            return "1280x720 (HD)";
        case Resolution::R1600x900:
            return "1600x900 (HD+)";
        case Resolution::R1920x1080:
            return "1920x1080 (Full HD)";
        case Resolution::R2560x1440:
            return "2560x1440 (QHD)";
        default:
            return "1920x1080 (Full HD)";
    }
}

/**
 * @brief Get resolution from width and height
 */
inline Resolution getResolutionFromSize(int width, int height) {
    if (width == 1280 && height == 720) return Resolution::R1280x720;
    if (width == 1600 && height == 900) return Resolution::R1600x900;
    if (width == 1920 && height == 1080) return Resolution::R1920x1080;
    if (width == 2560 && height == 1440) return Resolution::R2560x1440;
    return Resolution::R1920x1080;  // Default
}

}  // namespace rtype
