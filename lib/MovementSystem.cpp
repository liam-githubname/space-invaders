#include "MovementSystem.hpp"
#include "GameMechanics.hpp"
#include <cstdint>

class MovementSystem;

void MovementSystem::Update(GameState &game_state, uint64_t deltatime) {
  for (auto &entity : game_state.entities) {
    // I recognize that this is almost certainly a waste of resources
    if (entity.is_active && entity.transform.has_value() &&
        entity.velocity.has_value()) {
      entity.transform->x += entity.velocity->dx * deltatime;
      entity.transform->y += entity.velocity->dy * deltatime;
    }
  }
}
