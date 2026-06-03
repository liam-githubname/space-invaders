#pragma once

class GraphicsModule;
class GameState;
class AssetManager;

class UISystem {
private:
public:
  void Update(const GraphicsModule &graphics, GameState &game_state, const AssetManager &asset_manager) const;
};
