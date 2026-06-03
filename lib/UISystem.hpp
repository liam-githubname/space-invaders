#pragma once

class GraphicsModule;
class GameState;
class AssetManager;

class UISystem {
private:
public:
  void Update(GraphicsModule &graphics, GameState &game_state, AssetManager &asset_manager);
};
