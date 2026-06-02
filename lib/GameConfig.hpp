#pragma once
#include "Util.hpp"
#include <cstdint>
namespace GameConfig {
// ---------- Display ----------
inline constexpr int WINDOW_WIDTH = 256;
inline constexpr int WINDOW_HEIGHT = 224;
inline constexpr int TARGET_FPS = 60;
inline constexpr int FONT_SIZE_PT = 8;
// ---------- Colors ----------
struct RGBA {
  uint8_t r, g, b, a;
};
inline constexpr RGBA CLEAR_COLOR = {0, 0, 0, 255};
inline constexpr RGBA TEXT_WHITE = {255, 255, 255, 255};
inline constexpr RGBA SCORE_GREEN = {0, 255, 0, 255};
// ---------- Formation ----------
inline constexpr int FORMATION_ROWS = 5;
inline constexpr int FORMATION_COLS = 11;
inline constexpr float FORMATION_STRIDE_X = 16.0f;
inline constexpr float FORMATION_STRIDE_Y = 15.0f;
inline constexpr float FORMATION_START_X = 48.0f;
inline constexpr float FORMATION_START_Y = 40.0f;
inline constexpr float NEW_ROUND_OFFSET_Y = 8.0f;
inline constexpr int ALIENS_PER_ROUND = FORMATION_ROWS * FORMATION_COLS; // = 55
// ---------- Player ----------
inline constexpr Vec2 PLAYER_SPEED{1.0f, 0.0f};
inline constexpr int PLAYER_MAX_HP = 3;
inline constexpr Vec2 PLAYER_SPAWN{128.0f, 208.0f};
inline constexpr Vec2 PLAYER_DIRECTION = Up();
inline constexpr int STARTING_LIVES = 3;
// ---------- Aliens ----------
inline constexpr Vec2 ALIEN_SPEED{2.0f, 0.0f};
inline constexpr int ALIEN_MAX_HP = 1;
inline constexpr float ALIEN_BREACH_Y = 204.0f;
inline constexpr uint64_t ALIEN_STEP_BASE_NS = 500000000ULL;
inline constexpr uint64_t ALIEN_STEP_PER_KILL_NS = 6000000ULL;
inline constexpr int ALIEN_FIRE_CHANCE_1IN = 1000;
// ---------- Mystery Ship ----------
inline constexpr int MYSTERY_SHIP_SCORE = 200;
inline constexpr float MYSTERY_SHIP_SPEED = 0.75f;
inline constexpr float MYSTERY_SHIP_Y = 25.0f;
inline constexpr int MYSTERY_SHIP_MAX_HP = 1;
inline constexpr int MYSTERY_SHIP_SPAWN_TICKS = 4400;
// ---------- Bullets ----------
inline constexpr Vec2 PLAYER_BULLET_SPEED{0.0f, -3.0f};
inline constexpr float PLAYER_BULLET_OFFSET_Y = -7.0f;
inline constexpr Vec2 ENEMY_BULLET_SPEED{0.0f, 2.0f};
inline constexpr int BULLET_MAX_HP = 1;
// ---------- Barriers ----------
inline constexpr int BARRIER_COUNT = 4;
inline constexpr float BARRIER_SPACING_X = 56.0f;
inline constexpr Vec2 BARRIER_START{32.0f, 184.0f};
inline constexpr int BARRIER_PART_MAX_HP = 4;
inline constexpr float BARRIER_PART4_OFFSET_Y = 4.0f;
// ---------- Walls ----------
inline constexpr float WALL_THICKNESS = 1.0f;
inline constexpr float WALL_LR_OFFSET = 1.5f;
inline constexpr float WALL_BOTTOM_OFFSET = 0.5f;
// ---------- Scores ----------
inline constexpr int SCORE_SQUID = 30;
inline constexpr int SCORE_CRAB = 20;
inline constexpr int SCORE_OCTOPUS = 10;
inline constexpr int SCORE_SHIP = 200;
// ---------- UI ----------
inline constexpr float UI_HUD_Y = 1.0f;
inline constexpr float UI_SCORE_TITLE_X = 5.0f;
inline constexpr float UI_SCORE_NUM_X = 40.0f;
inline constexpr float UI_LIVES_TEXT_X = 120.0f;
inline constexpr float UI_LIVES_ICON_X = 155.0f;
inline constexpr float LIVES_PANEL_WIDTH = 120.0f;
inline constexpr float LIVES_PANEL_ROW_H = 14.0f;
inline constexpr float LIVES_COL_GAP = 2.0f;
inline constexpr float LIVES_ROW_GAP = 1.0f;
inline constexpr int LIVES_PER_ROW = 4;
inline constexpr int ROUND_TRANSITION_RENDERS = 150;
inline constexpr int LIVES_BONUS_PER_ROUND = 1;
inline constexpr float MENU_TITLE_SCALE = 2.0f;
inline constexpr float MENU_TITLE_FRAC_Y = 1.0f / 3.0f;
inline constexpr float MENU_PROMPT_FRAC_Y = 2.0f / 3.0f;
inline constexpr int ALIEN_DEATH_TICKS = 45;

} // namespace GameConfig
