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
// FIX:========================================================================
// ============================================================================
#include "ShootingSystem.hpp"
#include "Bitmask.hpp"
#include "Events.hpp"
#include "GameState.hpp"
#include "Raycast.hpp"
#include "Util.hpp"
#include <optional>

void ShootingSystem::Update(GameState &game_state) {

  for (auto &entity : game_state.entities) {
    // Make sure the entity has a transform
    if (!entity.transform)
      continue;
    // WARN: There has to be way to remove gun component from this system??
    // Check if the entity has a gun and if it's firing
    if (!entity.gun || !entity.gun->fire_flag)
      continue;

    // Bullet emission logic
    if (!game_state.bullet_is_active) {

      auto &player_bullet = game_state.CreateEntity();
      player_bullet.is_active = true;

      player_bullet.collider.emplace(Collider{.rect{1.0f, 1.5f}});
      player_bullet.velocity.emplace(Velocity{.speed = Up() * 2.0f});
      player_bullet.bitmask.emplace(Bitmask{
          .layer = GameLayer::Projectile,
          .mask = GameLayer::Wall | GameLayer::Enemy,
      });
      player_bullet.transform.emplace(
          Transform{.position = entity.transform->position, .direction = Up()});
      player_bullet.transform->position.y +=
          -2.0f * entity.transform->direction.y;

      game_state.bullet_is_active = true;
    }

    float shortest_distance = INFINITY;
    uint32_t hit_entity_id;

    // Raycast
    Raycast::Ray ray{.origin = Vec2{entity.transform->position.x,
                                    entity.transform->position.y},
                     .direction = Vec2{entity.transform->direction.x,
                                       entity.transform->direction.y}};

    for (auto &other_entity : game_state.entities) {

      // Skip if it's looking at itself
      if (entity.id == other_entity.id) {
        continue;
      }
      // skip if the caster isn't concerned with other entity's layer.
      if ((entity.bitmask->mask & other_entity.bitmask->layer) ==
          GameLayer::None) {
        continue;
      }

      if (other_entity.collider->shape == ColliderShape::Rectangle) {
        float min_x = other_entity.transform->position.x -
                      other_entity.collider->rect.width / 2;
        float max_x = other_entity.transform->position.x +
                      other_entity.collider->rect.width / 2;
        float min_y = other_entity.transform->position.y -
                      other_entity.collider->rect.height / 2;
        float max_y = other_entity.transform->position.y +
                      other_entity.collider->rect.height / 2;

        // NOTE: This is the first time I'm returning an optional like this.
        std::optional<float> distance =
            Raycast::RayAgainstAABB(ray, min_x, min_y, max_x, max_y);

        // I just learned that you don't need .has_value()
        // the optional has a built-in operator bool().
        // Which is a cool feature, but damn 2026-04-27.
        //
        if (!distance.has_value()) {
          continue;
        }
        if (*distance >= entity.gun->distance) {
          // SDL_Log("Distance value %f", *distance);
          continue;
        }

        // After further research, I could remove the .value() here as well.
        // With the addition of optionals pointer semantics was kept in
        // mind. so *distance is the same as distance.value();
        if (*distance < shortest_distance) {
          shortest_distance = distance.value();
          hit_entity_id = other_entity.id;
          // SDL_Log("shortest distance %f", shortest_distance);
        }
      }
      // TODO: #3 Check Circle colliders.
    }

    if (shortest_distance < entity.gun->distance) {
      //  WARN: These might be moving away from SRP, I'll have to rethink this
      game_state.event_queue.PushEvent(HitPayload{
          .entity_a_id = entity.id,
          .entity_b_id = hit_entity_id,
      });
    }
  }
}
// 2. Check non player shooting.
