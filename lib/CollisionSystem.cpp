#include "CollisionSystem.hpp"
#include "GameState.hpp"

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

      // Rectangle -> Rectangle
      if (entity_a.collider->shape == ColliderShape::Rectangle &&
          entity_b.collider->shape == ColliderShape::Rectangle) {
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
