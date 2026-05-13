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
// 3. I need to figure out if GameState.hpp should be included or not.
// FIXME:======================================================================
// ============================================================================

#include "EventSystem.hpp"
#include "Bitmask.hpp"
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
    std::visit(
        Overload{
            [&](const CollisionPayload payload) {
              HandleCollisionPayload(payload, game_state);
            },
            [&](const DeathPayload payload) {
              SDL_Log("Consumed DeathPayload");
            },
            [&](const ScorePayload payload) {
              SDL_Log("Consumed ScorePayload");
            },
            [&](const HitPayload payload) { SDL_Log("HitPayload event"); },
            //====================Add new payloads here===================
        },
        event);
  }
  game_state.event_queue.ClearEventQueue();
}

// Check the kind of components
void EventSystem::HandleCollisionPayload(const CollisionPayload &payload,
                                         GameState &game_state) {
  SDL_Log("In HandleCollisionPayload with %d and %d", payload.entity_a_id,
          payload.entity_b_id);

  // Handle Wall Collisions ====================================================
  float wall_x, wall_y;
  WallSide wallside;
  // WARN: Keeping a reference to a player seems like a slow down?
  Entity *player = nullptr;
  for (auto &entity : game_state.entities) {

    if (entity.id == payload.entity_a_id || entity.id == payload.entity_b_id) {
    }
  }
  // ==========================================================================
}

// Helper Functions ===========================================================
bool EventSystem::IsPlayerAndWall(const Entity &entity_a,
                                  const Entity &entity_b) {
  if (entity_a.bitmask->layer == GameLayer::Player &&
      entity_b.bitmask->layer == GameLayer::Wall) {
    return true;
  }
  if (entity_a.bitmask->layer == GameLayer::Wall &&
      entity_b.bitmask->layer == GameLayer::Player) {
    return true;
  }
  return false;
}

bool EventSystem::IsPlayerAndEnemy(const Entity &entity_a,
                                   const Entity &entity_b) {
  if (entity_a.bitmask->layer == GameLayer::Player &&
      entity_b.bitmask->layer == GameLayer::Enemy) {
    return true;
  }
  if (entity_a.bitmask->layer == GameLayer::Enemy &&
      entity_b.bitmask->layer == GameLayer::Player) {
    return true;
  }
  return false;
}

void EventSystem::WallCollision(Entity &entity_a, Entity &entity_b) {
  bool a_velocity = entity_a.velocity.has_value();
  bool b_velocity = entity_b.velocity.has_value();
  bool a_wall = entity_a.wall_info.has_value();
  bool b_wall = entity_b.wall_info.has_value();
  if (!(a_velocity && b_wall) && !(b_velocity && a_wall))
    return;

  // This is the easiest way to assign the player.
  auto player = (a_velocity) ? entity_a : entity_b;
  // This is redundant but a more elegant solution isn't coming to mind rn
  auto wall = (a_wall) ? entity_a : entity_b;

  float wall_x, wall_y;
  WallSide wallside;

  player.velocity->dy = -player.velocity->dy;
  player.velocity->dx = -player.velocity->dx;
  // FIXME: I feel like having to remember to add the transform and it's
  // colliders offset must be bad code.
  wall_x = wall.transform->x + wall.collider->offset_x;
  wall_y = wall.transform->y + wall.collider->offset_y;
  wallside = wall.wall_info->side;

  // WARN: The Eventsystem has to know what a wallside is, but I've made peace
  // with EventSystem being coupled with game logic.
  switch (wallside) {
  case WallSide::Top:
    player.transform->y = wall_y + player.collider->rect.height / 2;
    break;
  case WallSide::Bottom:
    player.transform->y = wall_y - player.collider->rect.height / 2;
    break;
  case WallSide::Left:
    player.transform->x = wall_x + player.collider->rect.width / 2;
    break;
  case WallSide::Right:
    player.transform->x = wall_x - player.collider->rect.width / 2;
    break;
  default:
    SDL_Log("in default switch case in EventSystem.cpp");
    break;
  }
}
