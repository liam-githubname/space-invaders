#pragma once

class GameState;
class Entity;

class CollisionSystem {
public:
  void Update(GameState &game_state);

private:
  bool check_rectangle_to_rectangle(Entity entity_a, Entity entity_b);
};
