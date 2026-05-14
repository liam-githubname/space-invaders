/* Events.hpp
 * Authored by Liam Harvell
 * I originally was housing this in the GameState file.
 * However, it had to be included by the CollisionSystem that needs to emit an
 * Event with a CollisionPayload. That meant that the CollisionSystem had to
 * include the GameState.
 * This is a better solution.
 */

// INFO:=======================================================================
// 1. I made the mistake of coupling game logic in with the Collision system.
//    By forcing the Collision system tell the EventSystem what kind
//    CollisionType had taken place, I forced CollisionSystem to have to be
//    updated for every new type. This violated SRP - 2026-04-27.
//      - It's funny how I could feel something wrong growing in this file, but
//      I couldn't quite figure out how to fix it yet.
// TODO:=======================================================================
// ============================================================================
#pragma once

#include <cstdint>
#include <variant>

// INFO: The reason I have class here is the enum must be accessed through the
// name CollisionType.

struct CollisionPayload {
  uint32_t entity_a_id;
  uint32_t entity_b_id;
};

struct DeathPayload {
  uint32_t entity_id;
};

struct ScorePayload {
  int points;
};

struct HitPayload {
  uint32_t entity_a_id;
  uint32_t entity_b_id;
};

// TODO: Write doc for clangd to tell me how to add Event variants when I
// inevitably forget.
// NOTE: std::variant is a typesafe union. I was considering
// using optional fields like how entities work, but after looking into it these
// seem to be a good option. This type alias Event causes every event to be size
// of the largest payload. I think there might be a more memory efficient way of
// doing this with anonymous unions?
using Event =
    std::variant<CollisionPayload, DeathPayload, ScorePayload, HitPayload>;
//  ^ #of times I forgot about this: 1
