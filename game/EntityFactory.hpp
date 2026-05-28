#pragma once
// TODO: EntityFactory - createPlayer(), createAlien(), createWall() -
// 2026-05-17

#include "GameState.hpp"
#include "Util.hpp"
class GameState;
class AssetManager;
enum class AlienType;

struct Config {
  int rows = 5;     // 5 rows of aliens
  int columns = 11; // 11 aliens per row (authentic to original)
  Vec2 first_alien_position = Vec2{.x = 24.0f, .y = 40.0f};
  Vec2 player_spawn_position = Vec2{.x = 112.0f, .y = 208.0f};
};

class EntityFactory {
public:
  EntityFactory(GameState &game_state, AssetManager &asset_manager,
                float window_width, float window_height);
  void createPlayer();
  void createGameWalls();
  void createAlien(AlienType species, Vec2 position);
  void createAlienFormation();

private:
  GameState &game_state_;
  Config config;
  AssetManager &asset_manager;
  float window_width_, window_height_;
};
