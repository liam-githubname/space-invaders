#include "SpaceInvadersMovementSystem.hpp"
#include "GameState.hpp"
#include <algorithm>

void SpaceInvadersMovementSystem::Update(GameState &game_state, float new_time,
                                         float dt) {

  // // Look for the player and update their movement
  // auto player = std::find_if(
  //     game_state.entities.begin(), game_state.entities.end(),
  //     [](Entity &entity) { return entity.player_input.has_value(); });
  // player->player_input->move_y = 0.0;
  //
  // for (auto &entity : game_state.entities) {
  //   if (!(entity.bitmask->layer == GameLayer::Enemy))
  //     continue;
  //   if (!entity.alien_info.has_value())
  //     continue;
  //
  //   entity.velocity->x_offset = 0.0;
  //   if ((new_time - timer) > (550000000ULL)) {
  //     if (entity.alien_info->type == AlienSpecies::Squid) {
  //       entity.transform->x +=
  //           (!entity.movement_mod.has_value())
  //               ? alien_movement_speed_x
  //               : alien_movement_speed_x * entity.movement_mod->speed_mod;
  //     }
  //     timer = new_time;
  //   }
  // }
};
