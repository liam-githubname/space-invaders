#pragma once
// TODO: EntityFactory - createPlayer(), createAlien(), createWall() -
// 2026-05-17

#include "GameState.hpp"
#include "Util.hpp"
class GameState;
enum class AlienType;

struct Config {
  int rows = 11;
  int columns = 5;
  Vec2 start_pos = Vec2{15.0f, 20.0f};
};

class EntityFactory {
public:
  EntityFactory(GameState &game_state, float window_width, float window_height);
  void createPlayer();
  void createGameWalls();
  void createAlien(AlienType species, Vec2 position);
  void createAlienFormation();

private:
  GameState &game_state_;
  Config config;
  float window_width_, window_height_;
};
