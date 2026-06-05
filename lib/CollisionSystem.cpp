/* CollisionSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 2. Colliders bounding box has to be accounted for around the origin of the
// entity. In other words you need to expand the collider to the half the height
// and width of the entity in both directions.
// 3. Only supports circles and AABB
// TODO:=======================================================================
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

void CollisionSystem::Update(GameState &game_state) const {
  for (int i = 0; i < game_state.entities.size(); i++) {
    const Entity &entity_a = game_state.entities[i];
    if (!entity_a.collider.has_value() || !entity_a.is_active) {
      continue;
    }
    const Collider &a_collider = entity_a.collider.value();

    for (int j = i + 1; j < game_state.entities.size(); j++) {
      bool is_colliding = false;
      const Entity &entity_b = game_state.entities[j];
      // Easy skip checks
      if (!entity_b.collider.has_value() || !entity_b.is_active) {
        continue;
      }
      const Collider &b_collider = entity_b.collider.value();

      // NOTE: This is not game logic, this is only checking if there is a
      // matching layer between the two entities and if there is check if the
      // two entities are colliding. I was having an issue seeing how this was
      // any different from holding gameplay logic. But it is the opposite this
      // is program logic, then game logic needs to work around CollisionSystems
      // rules. For example, when the player dies their mask should be set to
      // None, because they shouldn't be able to collide by anything anymore.
      if ((entity_a.bitmask->layer & entity_b.bitmask->mask) == GameLayer::None &&
          (entity_b.bitmask->layer & entity_a.bitmask->mask) == GameLayer::None) {
        continue;
      }

      // Rectangle -> Rectangle
      if (a_collider.shape == ColliderShape::Rectangle && b_collider.shape == ColliderShape::Rectangle) {
        is_colliding = IsRectToRectColliding(entity_a, entity_b);
      }

      // Circle -> Circle
      if (a_collider.shape == ColliderShape::Circle && b_collider.shape == ColliderShape::Circle) {
        is_colliding = IsCircleToCircleColliding(entity_a, entity_b);
      }

      // Rectnagle -> Circle || Circle -> Rectangle
      if (a_collider.shape == ColliderShape::Rectangle && b_collider.shape == ColliderShape::Circle) {
        is_colliding = IsRectToCircleColliding(entity_a, entity_b);
      }
      if (a_collider.shape == ColliderShape::Circle && b_collider.shape == ColliderShape::Rectangle) {
        is_colliding = IsRectToCircleColliding(entity_b, entity_a);
      }

      if (!is_colliding) {
        continue;
      }

      game_state.event_queue.PushEvent<CollisionEvent>(entity_a.id, entity_b.id);
    }
  }
}

bool CollisionSystem::IsRectToRectColliding(const Entity &entity_a, const Entity &entity_b) const {
  if (!entity_a.transform.has_value() || !entity_b.transform.has_value()) {
    SDL_Log("Entities don't have transform components");
    return false;
  }

  // locals to reduce dereferences
  const Vec2 &position_a = entity_a.transform->position;
  const Vec2 &position_b = entity_b.transform->position;
  const Collider &collider_a = entity_a.collider.value();
  const Collider &collider_b = entity_b.collider.value();

  const float a_left = position_a.x - collider_a.rect.width / 2;
  const float a_right = position_a.x + collider_a.rect.width / 2;
  const float a_top = position_a.y - collider_a.rect.height / 2;
  const float a_bottom = position_a.y + collider_a.rect.height / 2;
  const float b_left = position_b.x - collider_b.rect.width / 2;
  const float b_right = position_b.x + collider_b.rect.width / 2;
  const float b_top = position_b.y - collider_b.rect.height / 2;
  const float b_bottom = position_b.y + collider_b.rect.height / 2;

  if (a_right <= b_left || a_bottom <= b_top || a_left >= b_right || a_top >= b_bottom) {
    return false;
  }
  return true;
}

bool CollisionSystem::IsCircleToCircleColliding(const Entity &entity_a, const Entity &entity_b) const {
  const Vec2 &position_a = entity_a.transform->position;
  const Vec2 &position_b = entity_b.transform->position;

  const float center_to_center_dist =
    std::hypot(std::abs(position_a.x - position_b.x), std::abs(position_a.y - position_b.y));

  const float maximum_distance_apart = entity_a.collider->circle.radius + entity_b.collider->circle.radius;

  return center_to_center_dist <= maximum_distance_apart;
}

bool CollisionSystem::IsRectToCircleColliding(const Entity &rectangle_entity, const Entity &circle_entity) const {
  const Vec2 &rectangle_position = rectangle_entity.transform->position;
  const Vec2 &circle_position = circle_entity.transform->position;
  const Collider &collider_rect = rectangle_entity.collider.value();

  const float closest_point_of_rectangle_x =
    std::clamp(circle_position.x, rectangle_position.x - collider_rect.rect.width / 2,
               rectangle_position.x + collider_rect.rect.width / 2);
  const float closest_point_of_rectangle_y =
    std::clamp(circle_position.y, rectangle_position.y - collider_rect.rect.height / 2,
               rectangle_position.y + collider_rect.rect.height / 2);

  const float distance = std::hypot(std::abs(closest_point_of_rectangle_x - circle_position.x),
                                    std::abs(closest_point_of_rectangle_y - circle_position.y));

  return distance <= circle_entity.collider->circle.radius;
}
