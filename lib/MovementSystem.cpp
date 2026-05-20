// NOTE:=======================================================================
// 1. The movement system updates the transform based on the velocity.
// 2. The player entity uses the PlayerInput component.
// TODO:=======================================================================
// 1. Add multiderectional movement.
// 2. Normalize diagonal movement?
// ============================================================================
#include "MovementSystem.hpp"
#include "GameState.hpp"
#include <SDL3/SDL_log.h>

class MovementSystem;

void MovementSystem::Update(GameState &game_state, float current_frame_time) {

  auto has_been_half_second =
      ((current_frame_time - last_time) > (1000000000ULL));

  for (auto &entity : game_state.entities) {
    // Guard against dead entities.
    if (!entity.is_active) {
      continue;
    }
    // Update the transform component with the velocity component
    if (!entity.velocity.has_value() || !entity.transform.has_value()) {
      continue;
    }

    // If it hasnt been long enough and you're looking an alien stop looking.
    if (!has_been_half_second && entity.alien_info.has_value()) {
      continue;
    }

    auto new_transform_x_value = 0.0;
    auto new_transform_y_value = 0.0;

    // WARN: This won't work for anything but transform based movement.
    // Physics based movement isn't an easy swap.
    new_transform_x_value +=
        (entity.player_input && entity.velocity->speed)
            ? (entity.player_input->move_x * entity.velocity->speed.value())
            : entity.velocity->speed.value();

    if (entity.movement_mod.has_value()) {
      new_transform_x_value *= entity.movement_mod->speed_mod;
    }

    entity.transform->x += new_transform_x_value;
    entity.transform->y += new_transform_y_value;

    last_time = has_been_half_second ? current_frame_time : last_time;
  }
}
