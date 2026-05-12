#pragma once
#include "Events.hpp"

struct Entity;
class GameState;

// TODO: Write the gameplay logic for a collision event - For now have it emit a
// death payload event?
//

// TODO: Write
class EventSystem {
public:
  // TODO: Move the std::visitor for Events from main to here
  void ProcessEvents(GameState &game_state);

private:
  bool IsPlayerAndWall(const Entity &entity_a, const Entity &entity_b);
  bool IsPlayerAndEnemy(const Entity &entity_a, const Entity &entity_b);
  void DispatchEvent(const Event &event);
  void HandleCollisionPayload(const CollisionPayload &payload,
                              GameState &game_state);
};
