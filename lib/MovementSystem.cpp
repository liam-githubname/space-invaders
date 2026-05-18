// NOTE:=======================================================================
// 1. The movement system updates the transform based on the velocity.
// 2. The player entity uses the PlayerInput component.
// TODO:=======================================================================
// 1. Add multiderectional movement.
// 2. Normalize diagonal movement?
// ============================================================================
#include "MovementSystem.hpp"
#include "GameState.hpp"
#include "Util.hpp"
#include <SDL3/SDL_log.h>

class MovementSystem;

void MovementSystem::Update(GameState &game_state, float delta_time) {
  for (auto &entity : game_state.entities) {
    // Update the transform component with the velocity component
    if (entity.velocity.has_value() && entity.transform.has_value()) {
      entity.transform->y += entity.velocity->dy;
      entity.transform->x += entity.velocity->dx;
    }
  }
}
