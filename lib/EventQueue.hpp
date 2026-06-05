/* EventQueue.hpp
 * A small utility header file that holds the lightweight implementation of the
 * event queue. I debated whether or not to create this queue system or just
 * keep a raw vector<event>. But ultimately keeping it as it's own module means
 * purpose is better intimated. Implementation details can be updated without
 * refactoring Caller src code.
 *
 * When investigating a bug when events disappear you may want to put a debug
 * statement in the push function instead of hunting through all the push_back
 * calls.
 */
#pragma once

#include "Events.hpp"
#include <memory>
#include <vector>

class GameState;
struct Event;

class EventQueue {
private:
  std::vector<std::unique_ptr<Event>> event_queue;

public:
  // The const on the return type means callers can read and iterate, but can't modify.
  // The trailing const on the method means this is callable on a const EventQueue.
  void processEvents(GameState &game_state) const;
  const std::vector<std::unique_ptr<Event>> &GetEvents() const { return event_queue; };
  // void PushEvent(Event *const new_event) { event_queue.push_back(new_event); };
  template<typename T, typename... Args> //
  void PushEvent(Args &&...args) {
    event_queue.push_back(std::make_unique<T>(std::forward<Args>(args)...));
  }

  // This needs to call free on all of them.
  void ClearEventQueue(GameState &game_state);
};
