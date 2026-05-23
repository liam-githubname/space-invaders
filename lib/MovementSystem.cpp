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
      ((current_frame_time - last_time) > (500000000ULL));

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

    bool suppress = false;

    // The system sees an intent component
    if (entity.movement_mod.has_value()) {

      if (entity.movement_mod->transform_update.has_value()) {
        entity.transform->position.x = entity.movement_mod->transform_update->x;
        entity.transform->position.y = entity.movement_mod->transform_update->y;
      }

      entity.velocity->speed = entity.movement_mod->speed_mod;
      suppress = entity.movement_mod->suppress_velocity;

      entity.movement_mod = {};
    }

    if (!suppress) {
      // entity.transform->position.x +=
      //     (entity.player_input && entity.velocity)
      //         ? (entity.player_input->move.x * entity.velocity->speed.x)
      //         : entity.velocity->speed.x;

      auto new_entity_position = Zero();
      if (entity.player_input)
        entity

            entity.transform->position *=
            entity.transform->direction * entity.velocity->speed;
    }

    last_time = has_been_half_second ? current_frame_time : last_time;
  }
}
