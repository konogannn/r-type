/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** EmbeddedResources
*/

#pragma once
#include <cstddef>
#include <span>

namespace rtype {
namespace embedded {

#define ASSET_SPAN(data)                                                 \
    std::span<const std::byte>(reinterpret_cast<const std::byte*>(data), \
                               sizeof(data))

// Logo
static const unsigned char logo_data[] = {
#embed "../../../assets/icon/logo.png"
};

// Font
static const unsigned char font_data[] = {
#embed "../../../assets/fonts/default.ttf"
};

// Color filter
static const unsigned char color_filter_data[] = {
#embed "../../../assets/shaders/colorblind.frag"
};

// Music/Sound
static const unsigned char shot_sound_data[] = {
#embed "../../../assets/sound/shot_1.wav"
};

static const unsigned char hit_sound_data[] = {
#embed "../../../assets/sound/hit.wav"
};

static const unsigned char explosion_sound_data[] = {
#embed "../../../assets/sound/explosion.wav"
};

static const unsigned char click_sound_data[] = {
#embed "../../../assets/sound/clique_sound.wav"
};

static const unsigned char music_data[] = {
#embed "../../../assets/sound/menu_sound.wav"
};

// Background
static const unsigned char background_base_data[] = {
#embed "../../../assets/background/bg-back.png"
};

static const unsigned char background_stars_data[] = {
#embed "../../../assets/background/bg-stars.png"
};

static const unsigned char background_planet_data[] = {
#embed "../../../assets/background/bg-planet.png"
};

// Entities
static const unsigned char player_1_data[] = {
#embed "../../../assets/sprites/players/player1.png"
};

static const unsigned char player_2_data[] = {
#embed "../../../assets/sprites/players/player2.png"
};

static const unsigned char player_3_data[] = {
#embed "../../../assets/sprites/players/player3.png"
};

static const unsigned char player_4_data[] = {
#embed "../../../assets/sprites/players/player4.png"
};

static const unsigned char projectile_player_1_data[] = {
#embed "../../../assets/sprites/projectile_player_1.png"
};

static const unsigned char projectile_enemy_1_data[] = {
#embed "../../../assets/sprites/projectile_enemy_1.png"
};

static const unsigned char boss_1_data[] = {
#embed "../../../assets/sprites/boss_1.png"
};

static const unsigned char boss_2_data[] = {
#embed "../../../assets/sprites/boss_2.png"
};

static const unsigned char boss_3_data[] = {
#embed "../../../assets/sprites/boss_3.png"
};

static const unsigned char boss_4_data[] = {
#embed "../../../assets/sprites/boss_4.png"
};

static const unsigned char turret_data[] = {
#embed "../../../assets/sprites/turret.png"
};

static const unsigned char blowup_1_data[] = {
#embed "../../../assets/sprites/blowup_1.png"
};

static const unsigned char blowup_2_data[] = {
#embed "../../../assets/sprites/blowup_2.png"
};

}  // namespace embedded
}  // namespace rtype
