
#include "EventQueue.hpp"
#include "GameState.hpp"

void EventQueue::processEvents(GameState &game_state) const {
  for (const std::unique_ptr<Event> &event : game_state.event_queue.GetEvents()) {
    event->execute(game_state);
  }
  game_state.event_queue.ClearEventQueue(game_state);
}

void EventQueue::ClearEventQueue(GameState &game_state) {
  game_state.event_queue.event_queue.clear();
}
