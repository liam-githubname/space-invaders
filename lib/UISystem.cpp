
#include "UISystem.hpp"
#include "AssetManager.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"

void UISystem::Update(GraphicsModule &graphics, GameState &game_state,
                      AssetManager &asset_manager) {
  if (game_state.score_update) {
    asset_manager.createScoreNumber(graphics, game_state);
  }
}
