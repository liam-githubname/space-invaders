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

      // if the intent component has updates for position, apply those directly
      if (entity.movement_mod->position_update.has_value()) {
        entity.transform->position +=
            entity.movement_mod->position_update.value();
      }

      // if the new speed_assignment is there assign entities speed
      entity.velocity->speed =
          entity.movement_mod->speed_assignment.has_value()
              ? entity.movement_mod->speed_assignment.value()
              : entity.velocity->speed;

      suppress = entity.movement_mod->suppress_velocity;

      // clear the intent component
      entity.movement_mod = {};
    }

    auto new_entity_position = One();

    if (entity.player_input) {
      new_entity_position *= entity.player_input->move;
    }
    new_entity_position *= entity.velocity->speed;

    if (suppress) {
      continue;
    }

    entity.transform->position += new_entity_position;

    last_time = has_been_half_second ? current_frame_time : last_time;
  }
}
