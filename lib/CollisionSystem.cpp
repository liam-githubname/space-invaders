/* CollisionSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 2. Colliders bounding box has to be accounted for around the origin of the
// entity. In other words you need to expand the collider to the half the height
// and width of the entity in both directions.
// 3. Only supports circles and AABB
// TODO:=======================================================================
// #5 Move IsRectToRectColliding to a util file? It's just pure math.
// #7 Could implement OBB, for multiple shapes.
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

// This looks through all entity pairs that have colliders and are active to see
// if they have a collision.
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
      // TODO: #7 This isn't very readable, turn into constexpr?
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
      if (entity_a.collider->shape == ColliderShape::Rectangle &&
          entity_b.collider->shape == ColliderShape::Rectangle) {
        is_colliding = IsRectToRectColliding(entity_a, entity_b);
      }

      // Circle -> Circle
      if (entity_a.collider->shape == ColliderShape::Circle && entity_b.collider->shape == ColliderShape::Circle) {
        is_colliding = IsCircleToCircleColliding(entity_a, entity_b);
      }

      // Rectnagle -> Circle || Circle -> Rectangle
      if (entity_a.collider->shape == ColliderShape::Rectangle && entity_b.collider->shape == ColliderShape::Circle) {
        is_colliding = IsRectToCircleColliding(entity_a, entity_b);
      }
      if (entity_a.collider->shape == ColliderShape::Circle && entity_b.collider->shape == ColliderShape::Rectangle) {
        is_colliding = IsRectToCircleColliding(entity_b, entity_a);
      }

      if (!is_colliding) {
        continue;
      }

      // Designated Initializer syntax is fantastic
      game_state.event_queue.PushEvent(CollisionPayload{
        .entity_a_id = entity_a.id,
        .entity_b_id = entity_b.id,
      });
    }
  }
}

// TODO: #5
bool CollisionSystem::IsRectToRectColliding(const Entity &entity_a, const Entity &entity_b) const {
  const float a_left = entity_a.transform->position.x - entity_a.collider->rect.width / 2;
  const float a_right = entity_a.transform->position.x + entity_a.collider->rect.width / 2;
  const float a_top = entity_a.transform->position.y - entity_a.collider->rect.height / 2;
  const float a_bottom = entity_a.transform->position.y + entity_a.collider->rect.height / 2;
  const float b_left = entity_b.transform->position.x - entity_b.collider->rect.width / 2;
  const float b_right = entity_b.transform->position.x + entity_b.collider->rect.width / 2;
  const float b_top = entity_b.transform->position.y - entity_b.collider->rect.height / 2;
  const float b_bottom = entity_b.transform->position.y + entity_b.collider->rect.height / 2;

  if (a_right <= b_left || a_bottom <= b_top || a_left >= b_right || a_top >= b_bottom) {
    return false;
  }
  return true;
}

bool CollisionSystem::IsCircleToCircleColliding(const Entity &entity_a, const Entity &entity_b) const {
  const float center_to_center_dist =
    std::hypot(std::abs(entity_a.transform->position.x - entity_b.transform->position.x),
               std::abs(entity_a.transform->position.y - entity_b.transform->position.y));

  const float maximum_distance_apart = entity_a.collider->circle.radius + entity_b.collider->circle.radius;

  return (center_to_center_dist <= maximum_distance_apart) ? true : false;
}

bool CollisionSystem::IsRectToCircleColliding(const Entity &rectangle_entity, const Entity &circle_entity) const {
  const float closest_point_of_rectangle_x =
    std::clamp(circle_entity.transform->position.x,
               rectangle_entity.transform->position.x - rectangle_entity.collider->rect.width / 2,
               rectangle_entity.transform->position.x + rectangle_entity.collider->rect.width / 2);
  const float closest_point_of_rectangle_y =
    std::clamp(circle_entity.transform->position.y,
               rectangle_entity.transform->position.y - rectangle_entity.collider->rect.height / 2,
               rectangle_entity.transform->position.y + rectangle_entity.collider->rect.height / 2);

  const float distance = std::hypot(std::abs(closest_point_of_rectangle_x - circle_entity.transform->position.x),
                                    std::abs(closest_point_of_rectangle_y - circle_entity.transform->position.y));

  if (distance <= circle_entity.collider->circle.radius) {
    return true;
  }

  return false;
}
