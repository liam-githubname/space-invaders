#include "SpaceInvadersMovementSystem.hpp"
#include <algorithm>

void SpaceInvadersMovementSystem::Update(GameState &game_state) {
  auto player = std::find_if(
      game_state.entities.begin(), game_state.entities.end(),
      [](Entity &entity) { return entity.player_input.has_value(); });

  player->player_input->move_y = 0.0;
};
