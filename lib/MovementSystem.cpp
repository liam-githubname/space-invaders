#include "MovementSystem.hpp"
#include "GameMechanics.hpp"
#include <cstdint>
#include <iostream>

class MovementSystem;

void MovementSystem::Update(GameState &game_state, float deltatime) {
  for (auto &entity : game_state.entities) {
    // I recognize that this is almost certainly a waste of resources

    if (entity.is_active && entity.transform.has_value()) {
      entity.transform->x += entity.velocity->dx * deltatime;
      entity.transform->y += entity.velocity->dy * deltatime;

      std::cout << "trans->x: " << entity.transform->x
                << " vel->dx: " << entity.velocity->dx << std::endl;
    }
  }
}
