#pragma once

class GameState;
class AssetManager;
class Entity;

class MysteryShipSystem {

public:
  void Update(GameState &game_state, AssetManager &asset_manager,
              bool &is_running_);
};
