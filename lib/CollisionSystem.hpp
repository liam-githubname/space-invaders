#pragma once

class GameState;
class Entity;

class CollisionSystem {
public:
  void Update(GameState &game_state);

private:
  bool check_rectangle_to_rectangle(const Entity entity_a,
                                    const Entity entity_b);
  bool IsPlayerAndWall(const Entity &entity_a, const Entity &entity_b);
};
