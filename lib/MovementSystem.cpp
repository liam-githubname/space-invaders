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

void MovementSystem::Update(GameState &game_state, float delta_time) {
  for (auto &entity : game_state.entities) {
    // Guard against dead entities.
    if (!entity.is_active) {
      continue;
    }
    // Update the transform component with the velocity component
    if (entity.velocity.has_value() && entity.transform.has_value()) {

      auto new_transform_x_value = entity.transform->x;
      auto new_transform_y_value = entity.transform->y;

      if (entity.player_input.has_value()) {
        new_transform_x_value +=
            entity.player_input->move_x * entity.velocity->speed;
        new_transform_y_value +=
            entity.player_input->move_y * entity.velocity->speed;
      } else {
        new_transform_x_value += entity.velocity->x_offset;
        new_transform_y_value += entity.velocity->y_offset;
      }

      entity.transform->x = new_transform_x_value;
      entity.transform->y = new_transform_y_value;

      // entity.transform->y += entity.velocity->dy;
      // entity.transform->x += entity.velocity->dx;
    }
  }
}
