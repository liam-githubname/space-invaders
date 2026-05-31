#pragma once

#include "Events.hpp"
#include "GameState.hpp"
#include <SDL3/SDL_log.h>

struct Entity;
class GameState;

// TODO: Write the gameplay logic for a collision event - For now have it emit a
// death payload event?
//

class EventSystem {
public:
  void ProcessEvents(GameState &game_state);
  void HandleCollisionPayload(const CollisionPayload &payload,
                              GameState &game_state);
  void WallCollisionHandler(Entity &entity_a, Entity &entity_b,
                            GameState &game_state);

  void PlayerBulletCollisionHandler(Entity &entity_a, Entity &entity_b,
                                    GameState &game_state);
  void HandleDeathPayload() { SDL_Log("handleDeathPayload"); };
  void HandleScorePayload(const ScorePayload &payload, GameState &game_state);
  // void HandleHitPayload() { SDL_Log("handleHiyPayload"); };
};
