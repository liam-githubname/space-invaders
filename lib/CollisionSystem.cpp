#include "CollisionSystem.hpp"
#include "GameMechanics.hpp"

struct SDL_FRect;
class CollisionSystem;

void CollisionSystem::Update(GameState &state) {

  for (int i = 0; i < state.entities.size(); i++) {
    Entity &entityA = state.entities[i];
    if (!entityA.collider.has_value() || !entityA.is_active) {
      continue;
    }

    for (int j = i + 1; j < state.entities.size(); j++) {
      Entity &entityB = state.entities[j];
      if (!entityB.collider.has_value() || !entityB.is_active) {
        continue;
      }

      // Rectangle -> Rectangle
      if (entityA.collider->shape == ColliderShape::Rectangle &&
          entityB.collider->shape == ColliderShape::Rectangle) {
      }

      // Circle -> Circle
      if (entityA.collider->shape == ColliderShape::Circle &&
          entityB.collider->shape == ColliderShape::Circle) {
      }

      // Rectnagle -> Circle || Circle -> Rectangle
      if (entityA.collider->shape == ColliderShape::Rectangle &&
          entityB.collider->shape == ColliderShape::Circle) {
        // A = Rectangle & B = Circle logic
      }
      if (entityB.collider->shape == ColliderShape::Rectangle &&
          entityA.collider->shape == ColliderShape::Circle) {
        // B = Rectangle & A = Circle logic
      }
    }
  }
}
