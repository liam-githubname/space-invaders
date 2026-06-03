
#include "UISystem.hpp"
#include "AssetManager.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"

void UISystem::Update(const GraphicsModule &graphics, GameState &game_state, const AssetManager &asset_manager) const {
  if (game_state.score_update) {
    asset_manager.createScoreNumber(graphics, game_state);
  }
  if (game_state.lives_update) {
    asset_manager.createLivesCounter(graphics, game_state);
  }
}
