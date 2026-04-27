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
// #1 Is shootingSystem going to iterate over all entities? Should there be a
// raycasts vector? I don't think so, the right answer should be that the
// shootingSystem should iterate over all entities, if the entity can shoot and
// is shooting then it should grab the information that will define the ray.
// If it's the player the shooting distance.
// #2 Move the player update functionality into a new function. Because it's
// nested in like a billion braces.
// #3 add the circle collider check.
// #4 add other payload types.
// FIX:========================================================================
// #1 URGENT FIX: I have an enum for the entity_type in here. VERY BAD
// coupling, will remove later.
// ============================================================================
#include "ShootingSystem.hpp"
#include "Events.hpp"
#include "GameState.hpp"
#include "Raycast.hpp"
#include <optional>

void ShootingSystem::Update(GameState &game_state) {
  float shortest_distance = INFINITY;
  uint32_t hit_entity_id;
  // FIX: #1 This doesn't follow style ON PURPOSE, FIX THIS SHIT lol
  //      Wait this is a non issue I just need to check if the entity
  enum class Entity_type { player_enemy, player_wall };

  Entity_type return_entity_type;

  for (auto &entity : game_state.entities) {
    if (!entity.gun.has_value())
      continue;

    // 1. check players shooting
    if (entity.is_player.has_value()) {
      if (entity.player_input.has_value() && entity.player_input->is_firing &&
          !entity.direction.has_value()) {
        Raycast::Ray ray{.origin_x = entity.transform->x,
                         .origin_y = entity.transform->y,
                         .direction_x = 0.0f,
                         .direction_y = 1.0f};

        for (auto &other_entity : game_state.entities) {
          if (entity.id == other_entity.id ||
              !other_entity.collider.has_value()) {
            continue;
          }
          if (other_entity.collider->shape == ColliderShape::Rectangle) {
            float min_x = other_entity.transform->x -
                          other_entity.collider->rect.width / 2;
            float max_x = other_entity.transform->x +
                          other_entity.collider->rect.width / 2;
            float min_y = other_entity.transform->y -
                          other_entity.collider->rect.height / 2;
            float max_y = other_entity.transform->y +
                          other_entity.collider->rect.height / 2;

            // NOTE: This is the first time I'm returning an optional like this.
            std::optional<float> distance =
                Raycast::RayAgainstAABB(ray, min_x, min_y, max_x, max_y);

            // I just learned that you don't need .has_value()
            // the optional has a built-in operator bool().
            // Which is a cool feature, but damn 2026-04-27.
            if (!distance.has_value() || *distance >= entity.gun->distance) {
              continue;
            }

            // After further research, I could remove the .value() here as well.
            // With the addition of optionals pointer semantics was kept in
            // mind. so *distance is the same as distance.value();
            if (*distance < shortest_distance) {
              shortest_distance = distance.value();
              hit_entity_id = other_entity.id;

              if (other_entity.is_enemy) {
                return_entity_type = Entity_type::player_enemy;
              }

              if (other_entity.is_wall) {
                return_entity_type = Entity_type::player_wall;
              }
            }
          }
          // TODO: #3 Check Circle colliders.
        }

        if (return_entity_type == Entity_type::player_enemy) {
          game_state.event_queue.PushEvent(
              HitPayload{.entity_a_id = entity.id,
                         .entity_b_id = hit_entity_id,
                         .hit_type = HitType::PlayerAndEnemy});
        } else {
          // TODO: #4 replace with real logic.
          game_state.event_queue.PushEvent(
              HitPayload{.entity_a_id = entity.id,
                         .entity_b_id = hit_entity_id,
                         .hit_type = HitType::PlayerAndEnemy});
        }
      }
    }
    // 2. Check non player shooting.
  }
}
