#pragma once
// TODO: EntityFactory - createPlayer(), createAlien(), createWall() -
// 2026-05-17

#include "GameState.hpp"
#include "Util.hpp"
class GameState;
class AssetManager;
enum class AlienType;

class EntityFactory {
public:
  EntityFactory(GameState &game_state, AssetManager &asset_manager,
                float window_width, float window_height);
  void createPlayer();
  void createBarrier(Vec2 position);
  void createBarriers();
  void createGameWalls();
  void createAlien(AlienType species, Vec2 position);
  void createAlienFormation(Vec2 position);
  void createMysteryShipSpawner();

private:
  GameState &game_state_;
  AssetManager &asset_manager;
  float window_width_, window_height_;
};
