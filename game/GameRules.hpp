#pragma once
// TODO: GameRules - event handlers for wall collisions, death, score -
// 2026-05-17
#include "Events.hpp"
#include "GameState.hpp"

#include <SDL3/SDL_log.h>

class GameRules {
public:
  void HandleCollisionPayload(const CollisionPayload &payload,
                              GameState &game_state);
  void WallCollisionHandler(Entity &entity_a, Entity &entity_b);

  void HandleDeathPayload() { SDL_Log("handleDeathPayload"); };
  void HandleScorePayload() { SDL_Log("handlescorepayload"); };
  void HandleHitPayload() { SDL_Log("handleHiyPayload"); };
};
