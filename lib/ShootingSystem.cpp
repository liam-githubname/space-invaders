/* ShootingSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. Raycast.hpp has the definition of a raycast as well as the equation.
// 2. The raycast functions are not normalized right now. That needs to be fixed
// 3. Very important the Raycast functions return the distance if a hit occurred
// otherwise it returns a nullopt. The shooting system will check if that
// distance is too far. This removes game knoweledge burden from Raycast.hpp.
// TODO:=======================================================================
// 1. Is shootingSystem going to iterate over all entities? Should there be a
// raycasts vector? I don't think so, the right answer should be that the
// shootingSystem should iterate over all entities, if the entity can shoot and
// is shooting then it should grab the information that will define the ray.
// If it's the player the shooting distance.
// ============================================================================
#include "ShootingSystem.hpp"
#include "GameState.hpp"
#include "Raycast.hpp"

void ShootingSystem::Update(GameState &game_state) {
  for (auto &entity : game_state.entities) {
    // 1. check players shooting
    if (!entity.gun.has_value())
      continue;

    // Theoretically player input shouldn't ever be empty but whatever.
    if (entity.is_player.has_value()) {
      if (entity.player_input.has_value() && entity.player_input->is_firing &&
          !entity.direction.has_value()) {
        Raycast::Ray ray{.origin_x = entity.transform->x,
                         .origin_y = entity.transform->y,
                         .direction_x = 0.0f,
                         .direction_y = 1.0f};

        for (auto &other_entity : game_state.entities) {
          if (entity.id == other_entity.id) {
            continue;
          }
        }
      }
    }
    // 2. Check non player shooting.
  }
}
