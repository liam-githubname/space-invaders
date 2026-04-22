#pragma once

#include "EventQueue.hpp"

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
  void DispatchEvent(Event &event);
};
