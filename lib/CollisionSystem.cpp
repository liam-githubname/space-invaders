/* CollisionSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. Colliders origin is in the top left of their bounding box naturally.
// TODO:=======================================================================
// #4 Move IsPlayerAndWall to a util file? I will do this if I end up needing it
//    somewhere else.
// #5 Move IsRectToRectColliding to a util file? It's just pure math.
// #6 I don't like how I have to have a separate branch for each combination of
// types. I have tried to reduce the nesting and specific conditions required to
// understand the state of the collision system by the time there is a detected
// collision.
// FIX:========================================================================
// ============================================================================

#include "CollisionSystem.hpp"
#include "Bitmask.hpp"
#include "Events.hpp"
#include "GameState.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>

struct SDL_FRect;
class GameState;

void CollisionSystem::Update(GameState &game_state) {

  for (int i = 0; i < game_state.entities.size(); i++) {
    Entity &entity_a = game_state.entities[i];
    if (!entity_a.collider.has_value() || !entity_a.is_active) {
      continue;
    }

    for (int j = i + 1; j < game_state.entities.size(); j++) {
      bool is_colliding = false;
      Entity &entity_b = game_state.entities[j];

      // Easy skip checks
      if (!entity_b.collider.has_value() || !entity_b.is_active) {
        continue;
      }
      if (entity_a.bitmask->layer == GameLayer::Wall &&
          entity_b.bitmask->layer == GameLayer::Wall) {
        continue;
      }

      // Rectangle -> Rectangle
      if (entity_a.collider->shape == ColliderShape::Rectangle &&
          entity_b.collider->shape == ColliderShape::Rectangle) {

        is_colliding = IsRectToRectColliding(entity_a, entity_b);
      }

      // Circle -> Circle
      if (entity_a.collider->shape == ColliderShape::Circle &&
          entity_b.collider->shape == ColliderShape::Circle) {
        is_colliding = IsCircleToCircleColliding(entity_a, entity_b);
      }

      // Rectnagle -> Circle || Circle -> Rectangle
      if (entity_a.collider->shape == ColliderShape::Rectangle &&
          entity_b.collider->shape == ColliderShape::Circle) {
        is_colliding = IsRectToCircleColliding(entity_a, entity_b);
      }
      if (entity_a.collider->shape == ColliderShape::Circle &&
          entity_b.collider->shape == ColliderShape::Rectangle) {
        is_colliding = IsRectToCircleColliding(entity_b, entity_a);
      }

      if (!is_colliding) {
        continue;
      }

      // We know there is a collision by here. Time to figure out what kind
      // Wrong we don't need to know what kind, That BREAKS SRP bruh
      //
      // TODO: #6 This seems like a non-exclusive type components problem.
      // Designated Initializer syntax is fantastic
      game_state.event_queue.PushEvent(CollisionPayload{
          .entity_a_id = entity_a.id,
          .entity_b_id = entity_b.id,
      });
    }
  }
}

// bool CollisionSystem::IsPlayerAndWall(const Entity &entity_a,
//                                       const Entity &entity_b) {
//   if (entity_a.is_player.has_value() && entity_b.is_wall.has_value()) {
//     return true;
//   }
//   if (entity_a.is_wall.has_value() && entity_b.is_player.has_value()) {
//     return true;
//   }
//   return false;
// }
//
// bool CollisionSystem::IsPlayerAndEnemy(const Entity &entity_a,
//                                        const Entity &entity_b) {
//   if (entity_a.is_player.has_value() && entity_b.is_enemy.has_value()) {
//     return true;
//   }
//   if (entity_a.is_enemy.has_value() && entity_b.is_player.has_value()) {
//     return true;
//   }
//   return false;
// }
//
// TODO: #5
bool CollisionSystem::IsRectToRectColliding(const Entity &entity_a,
                                            const Entity &entity_b) {

  auto a_left = entity_a.transform->x - entity_a.collider->rect.width / 2;
  auto a_right = entity_a.transform->x + entity_a.collider->rect.width / 2;
  auto a_top = entity_a.transform->y - entity_a.collider->rect.height / 2;
  auto a_bottom = entity_a.transform->y + entity_a.collider->rect.height / 2;
  auto b_left = entity_b.transform->x - entity_b.collider->rect.width / 2;
  auto b_right = entity_b.transform->x + entity_b.collider->rect.width / 2;
  auto b_top = entity_b.transform->y - entity_b.collider->rect.height / 2;
  auto b_bottom = entity_b.transform->y + entity_b.collider->rect.height / 2;

  if (a_right <= b_left || a_bottom <= b_top || a_left >= b_right ||
      a_top >= b_bottom) {
    return false;
  }
  return true;
}

bool CollisionSystem::IsCircleToCircleColliding(const Entity &entity_a,
                                                const Entity &entity_b) {
  float center_to_center_dist =
      std::hypot(std::abs(entity_a.transform->x - entity_b.transform->y),
                 std::abs(entity_a.transform->y - entity_b.transform->y));

  float maximum_distance_apart =
      entity_a.collider->circle.radius + entity_b.collider->circle.radius;

  return (center_to_center_dist <= maximum_distance_apart) ? true : false;
}

bool CollisionSystem::IsRectToCircleColliding(const Entity &rectangle_entity,
                                              const Entity &circle_entity) {

  float closest_point_of_rectangle_x = std::clamp(
      circle_entity.transform->x,
      rectangle_entity.transform->x - rectangle_entity.collider->rect.width / 2,
      rectangle_entity.transform->x +
          rectangle_entity.collider->rect.width / 2);
  float closest_point_of_rectangle_y = std::clamp(
      circle_entity.transform->y,
      rectangle_entity.transform->y - rectangle_entity.collider->rect.width / 2,
      rectangle_entity.transform->y +
          rectangle_entity.collider->rect.width / 2);

  float distance = std::hypot(
      std::abs(closest_point_of_rectangle_x - circle_entity.transform->x),
      std::abs(closest_point_of_rectangle_y - circle_entity.transform->y));

  if (distance <= circle_entity.collider->circle.radius) {
    return true;
  }

  return false;
}
