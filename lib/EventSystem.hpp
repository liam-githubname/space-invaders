#pragma once

#include "GameState.hpp"

struct Entity;
class GameState;

// TODO: Write the gameplay logic for a collision event - For now have it emit a
// death payload event?
//

class EventSystem {
public:
  // TODO: Move the std::visitor for Events from main to here
  // Explain this template here!
  template <typename Visitor>
  void ProcessEvents(GameState &game_state, Visitor &&visitor) {
    for (auto &event : game_state.event_queue.GetEvents()) {
      // WARN: This auto &&payload syntax can accept
      // anything, this is probably why clangd can't help out here.
      std::visit(
          // Overload{
          //     [&](const CollisionPayload payload) {
          //       HandleCollisionPayload(payload, game_state);
          //     },
          //     [&](const DeathPayload payload) {
          //       SDL_Log("Consumed DeathPayload");
          //     },
          //     [&](const ScorePayload payload) {
          //       SDL_Log("Consumed ScorePayload");
          //     },
          //     [&](const HitPayload payload) { SDL_Log("HitPayload event"); },
          //     //====================Add new payloads here===================
          // },
          std::forward<Visitor>(visitor), event);
    }
    game_state.event_queue.ClearEventQueue();
  }

  // Moving gameplay logic to separate gameplay directory
private:
};
