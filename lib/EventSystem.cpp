/* EventSystem.cpp
 * Authored by Liam Harvell
 * ====================== notes ========================================
 *  std::visit combined with std::variant gives you polymorphism without
 *  pointers, exhaustiveness checking, and zero overhead.
 *  According to AI there is a common idiom for handling this. the Overload
 *  pattern
 *
 *  In C++, every lambda expression creates a unique, anonymous struct type
 *  with an operator() member
 *
 *  INFO: Operator overloading is a complicated subset of c++, look at
 *  "operators in c++" note in obsidian vault.
 *  Using a generic Lambda expression with the if constexpr
 *  INFO: The "if constexpr" forces the compiler to only generate only the
 *  matching branch is compiled for each event type. The others will be
 *  discarded at compile time
 */

#include "EventSystem.hpp"
#include "GameState.hpp"
// TODO: Remove after the logging isn't required
#include <SDL3/SDL_log.h>

class EventSystem;

// This is pulled directly from
// https://en.cppreference.com/cpp/utility/variant/visit2 @ 2026-04-22 14:45
// A *Variadic template*. The ... is called a parameter pack.
// "This template accepts any number of type arguments by the name Ts"
// struct overloaded : Ts... "multiple inheritance" via pack expansion.
// "the struct [overloaded] ihnherits from every type in Ts simultaneously"
// but C++ templates are a compile-time code generation mechanism

// This idiom exists because C++ doesn't have a built-in way to create an
// overload set from multiple lambdas.
template <typename... Ts> struct Overload : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> Overload(Ts...) -> Overload<Ts...>;

// This is the main use of this class here.
void EventSystem::ProcessEvents(GameState &game_state) {
  for (auto &event : game_state.event_queue.GetEvents()) {
    // WARN: This auto &&payload syntax can accept
    // anything, this is probably why clangd can't help out here.
    std::visit(
        Overload{[&](const CollisionPayload payload) {
                   SDL_Log("Collision event - entity_a_id: %d, entity_b_id: %d",
                           payload.entity_a_id, payload.entity_b_id);
                 },
                 [&](const DeathPayload payload) {
                   SDL_Log("Consumed DeathPayload");
                 },
                 [&](const ScorePayload payload) {
                   SDL_Log("Consumed ScorePayload");
                 }},
        event);
  }
  game_state.event_queue.ClearEventQueue();
}

// Check the kind of components
void HandleCollisionPayload(const CollisionPayload &payload) {}
