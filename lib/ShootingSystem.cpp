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
// #1 URGENT FIX I have an enum for the entity_type in here. VERY BAD
// coupling, will remove later.
// #2 I should try and decouple the shooting system from gameplay logic. I need
// to figure out how to have a separation of concerns.
// ============================================================================
#include "ShootingSystem.hpp"
#include "Events.hpp"
#include "GameState.hpp"
#include "Raycast.hpp"
#include <optional>

void ShootingSystem::Update(GameState &game_state) {
  float shortest_distance = INFINITY;
  uint32_t hit_entity_id;
  // FIX: #1 This doesn't follow style ON PURPOSE, FIX THIS lol
  //      Wait this is a non issue I just need to check if the entity
  enum class Entity_type { player_enemy, player_wall };

  Entity_type return_entity_type;

  for (auto &entity : game_state.entities) {
    // Make sure the entity has a transform
    if (!entity.transform)
      continue;
    // Check if the entity has a gun and if it's firing
    if (!entity.gun || !entity.gun->fire_flag)
      continue;

    // 1. check players shooting
    Raycast::Ray ray{.origin_x = entity.transform->x,
                     .origin_y = entity.transform->y,
                     .direction_x = entity.transform->direction_x,
                     .direction_y = entity.transform->direction_y};

    for (auto &other_entity : game_state.entities) {
      if (entity.id == other_entity.id || !other_entity.collider.has_value()) {
        continue;
      }
      if (other_entity.collider->shape == ColliderShape::Rectangle) {
        float min_x =
            other_entity.transform->x - other_entity.collider->rect.width / 2;
        float max_x =
            other_entity.transform->x + other_entity.collider->rect.width / 2;
        float min_y =
            other_entity.transform->y - other_entity.collider->rect.height / 2;
        float max_y =
            other_entity.transform->y + other_entity.collider->rect.height / 2;

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

          if (other_entity.bitmask->layer == GameLayer::Enemy) {
            return_entity_type = Entity_type::player_enemy;
          }

          if (other_entity.bitmask->layer == GameLayer::Wall) {
            return_entity_type = Entity_type::player_wall;
          }
        }
      }
      // TODO: #3 Check Circle colliders.
    }

    // WARN: These might be moving away from SRP, I'll have to rethink this
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
