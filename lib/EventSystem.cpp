/* EventSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. std::visit combined with std::variant gives you polymorphism without
// pointers, exhaustiveness checking, and zero overhead.
// 2. According to AI there is a common idiom for handling this. the Overload
// pattern
// 3. In C++, every lambda expression creates a unique, anonymous struct type
// with an operator() member
// 4. Operator overloading is a complicated subset of c++, look at
// "operators in c++" note in obsidian vault.
// Using a generic Lambda expression with the if constexpr
// 5. The "if constexpr" forces the compiler to only generate only the
// matching branch is compiled for each event type. The others will be
// discarded at compile time
// TODO:=======================================================================
// 1. Remove the magic numbers in the wall collision handler.
// ============================================================================

#include "EventSystem.hpp"
#include "Events.hpp"
#include "GameState.hpp"
// TODO: Remove after the logging isn't required
#include <SDL3/SDL_log.h>
#include <cstdlib>

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
    std::visit(Overload{[&](const CollisionPayload payload) {
                          HandleCollisionPayload(payload, game_state);
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
void EventSystem::HandleCollisionPayload(const CollisionPayload &payload,
                                         GameState &game_state) {
  // FIXME: Make sure that this get filled with references.
  SDL_Log("In HandleCollisionPayload with %d and %d", payload.entity_a_id,
          payload.entity_b_id);
  float wall_x, wall_y;
  Entity *player = nullptr;
  switch (payload.collision_type) {
  case CollisionType::PlayerAndWall:
    for (auto &entity : game_state.entities) {
      if (entity.id == payload.entity_a_id ||
          entity.id == payload.entity_b_id) {
        if (entity.is_player.has_value()) {
          entity.velocity->dy = -entity.velocity->dy;
          entity.velocity->dx = -entity.velocity->dy;
          player = &entity;
        } else {
          // FIXME: I feel like having to remember to add the transform and it's
          // colliders offset must be bad code.
          wall_x = entity.transform->x + entity.collider->offset_x;
          wall_y = entity.transform->y + entity.collider->offset_y;
        }
      }
    }
    // FIXME: Remove magic numbers, figure out how to remove later.
    // FIXME: create more robust system than having to add collider width /2.
    if (payload.entity_b_id == 1) {
      player->transform->y = wall_y + player->collider->rect.width / 2;
    }
    if (payload.entity_b_id == 2) {
      player->transform->y = wall_y - player->collider->rect.width / 2;
    }
    if (payload.entity_b_id == 3) {
      player->transform->x = wall_x + player->collider->rect.height / 2;
    }
    if (payload.entity_b_id == 4) {
      player->transform->x = wall_x - player->collider->rect.height / 2;
    }
    break;

  case CollisionType::PlayerAndEnemy:
    SDL_Log("Woah that's crazy I haven't made an enemy yet");
  }
}
