#pragma once
// TODO: EntityFactory - createPlayer(), createAlien(), createWall() -
// 2026-05-17

#include "GameState.hpp"
class GameState;
enum class AlienType;

class EntityFactory {
public:
  EntityFactory();
  EntityFactory(GameState &game_state, float window_width, float window_height);
  void createPlayer();
  void createGameWalls();
  void createAlien(AlienType species, Vec2 position);
  void createAlienFormation();

private:
  GameState &game_state_;
  float window_width_, window_height_;
  float width_unit = (float)window_width_ / 44;
  float height_unit = (float)window_height_ / 44;
};
