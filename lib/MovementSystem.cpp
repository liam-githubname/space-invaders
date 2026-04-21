#include "MovementSystem.hpp"
#include "GameState.hpp"

class MovementSystem;

void MovementSystem::Update(GameState &game_state, float delta_time) {
  for (auto &entity : game_state.entities) {
    // TODO: Add bounding to the edge of the world? Must have to do this through
    // the GameState - AI is telling me that this would violate separation of
    // concerns
    //

    // I recognize that this is almost certainly a waste of resources
    if (entity.is_active && entity.transform.has_value() &&
        entity.velocity.has_value()) {
      entity.transform->x += entity.velocity->dx * delta_time;
      entity.transform->y += entity.velocity->dy * delta_time;
    }
  }
}
