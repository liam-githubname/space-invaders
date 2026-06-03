#pragma once

class GameState;
class Entity;

class CollisionSystem {
public:
  void Update(GameState &game_state);

private:
  bool IsRectToRectColliding(const Entity &entity_a, const Entity &entity_b) const;

  bool IsRectToCircleColliding(const Entity &rectangle_entity, const Entity &circle_entity) const;

  bool IsCircleToCircleColliding(const Entity &entity_a, const Entity &entity_b) const;
};
