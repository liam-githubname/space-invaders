#include "CollisionSystem.hpp"
#include "GameState.hpp"
#include <SDL3/SDL.h>
#include <algorithm>

// TODO: Implement collision detection for circle -> circle
// TODO: Implement collision detection for rectangle -> circle

struct SDL_FRect;
class CollisionSystem;

void CollisionSystem::Update(GameState &game_state) {

  for (int i = 0; i < game_state.entities.size(); i++) {
    Entity &entity_a = game_state.entities[i];
    if (!entity_a.collider.has_value() || !entity_a.is_active) {
      continue;
    }

    for (int j = i + 1; j < game_state.entities.size(); j++) {
      Entity &entity_b = game_state.entities[j];
      if (!entity_b.collider.has_value() || !entity_b.is_active) {
        continue;
      }

      bool is_colliding = true;
      // The Colliders count from the top right of the entity they are attached
      // to. Rectangle -> Rectangle
      if (entity_a.collider->shape == ColliderShape::Rectangle &&
          entity_b.collider->shape == ColliderShape::Rectangle) {
        SDL_Log("Rect -> Rect");
        auto a_left = entity_a.transform->x;
        auto a_right = entity_a.transform->x + entity_a.collider->rect.width;
        auto a_top = entity_a.transform->y;
        auto a_bottom = entity_a.transform->y + entity_a.collider->rect.height;
        auto b_left = entity_b.transform->x;
        auto b_right = entity_b.transform->x + entity_b.collider->rect.width;
        auto b_top = entity_b.transform->y;
        auto b_bottom = entity_b.transform->y + entity_b.collider->rect.height;

        // A is completely to the left of B
        if (a_right <= b_left || a_bottom <= b_top || a_left >= b_right ||
            a_top >= b_bottom) {
          is_colliding = false;
          continue;
        }

        // NOTE: This designated initialization syntax is incredible. It is so
        // readable.
        game_state.events.push_back(CollisionPayload{
            .entity_a_id = entity_a.id, .entity_b_id = entity_b.id});
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
      if (entity_b.collider->shape == ColliderShape::Rectangle &&
          entity_a.collider->shape == ColliderShape::Circle) {
        // B = Rectangle & A = Circle logic
      }
    }
  }
}
