/* CollisionSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. Colliders origin is in the top left of their bounding box.
// TODO:=======================================================================
// #2 Implement collision detection for circle -> circle.
// #3 Implement collision detection for rectangle -> circle.
// #4 Move IsPlayerAndWall to a util file? I will do this if I end up needing it
//    somewhere else.
// #5 Move IsRectToRectColliding to a util file? It's just pure math.
// ============================================================================
#include "CollisionSystem.hpp"
#include "Events.hpp"
#include "GameState.hpp"
#include <SDL3/SDL.h>

struct SDL_FRect;
class GameState;

void CollisionSystem::Update(GameState &game_state) {

  for (int i = 0; i < game_state.entities.size(); i++) {
    Entity &entity_a = game_state.entities[i];
    if (!entity_a.collider.has_value() || !entity_a.is_active) {
      continue;
    }

    for (int j = i + 1; j < game_state.entities.size(); j++) {
      Entity &entity_b = game_state.entities[j];
      //
      if (!entity_b.collider.has_value() || !entity_b.is_active) {
        continue;
      }

      // WORKING
      if (entity_a.is_wall.has_value() && entity_b.is_wall.has_value()) {
        continue;
      }

      // Rectangle -> Rectangle
      if (!IsRectToRectColliding(entity_a, entity_b)) {
        continue;
      }

      // We know there is a collision by here. Time to figure out what kind
      if (IsPlayerAndWall(entity_a, entity_b)) {
        // Designated Initializer syntax is fantastic
        game_state.event_queue.PushEvent(
            CollisionPayload{.entity_a_id = entity_a.id,
                             .entity_b_id = entity_b.id,
                             .collision_type = CollisionType::PlayerAndWall});
      }

      // Circle -> Circle
      if (entity_a.collider->shape == ColliderShape::Circle &&
          entity_b.collider->shape == ColliderShape::Circle) {
      }

      // Rectnagle -> Circle || Circle -> Rectangle
      if (entity_a.collider->shape == ColliderShape::Rectangle &&
          entity_b.collider->shape == ColliderShape::Circle) {
        // A = Rectangle & B = Circle logic
      }
      if (entity_a.collider->shape == ColliderShape::Circle &&
          entity_b.collider->shape == ColliderShape::Rectangle) {
        // B = Rectangle & A = Circle logic
      }
    }
  }
}

bool CollisionSystem::IsPlayerAndWall(const Entity &entity_a,
                                      const Entity &entity_b) {
  if (entity_a.is_player.has_value() && entity_b.is_wall.has_value()) {
    return true;
  }
  if (entity_a.is_wall.has_value() && entity_b.is_player.has_value()) {
    return true;
  }
  // FIXME: remove
  SDL_Log("returning false from IsPlayerAndWall");
  return false;
}

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
