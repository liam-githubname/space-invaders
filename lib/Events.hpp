/* Events.hpp
 * Authored by Liam Harvell
 * I originally was housing this in the GameState file.
 * However, it had to be included by the CollisionSystem that needs to emit an
 * Event with a CollisionPayload. That meant that the CollisionSystem had to
 * include the GameState.
 * This is a better solution.
 */

// INFO:=======================================================================
// 1.
// TODO:=======================================================================
// #1 The collision and hit types feel like they are redundent to each other.
// Although I think they should stay separate, as they might diverge a little.
// More importantly though I'm thinking that they are specific types that
// entities can be. When/If I get around to creating a union around the IsType
// components on entities I may need to change the way that these work.
// ============================================================================
// define a style for these payloads
// The "payloads", this is where events are defined. Events are
#pragma once

#include <cstdint>
#include <variant>

// INFO: The reason I have class here is the enum must be accessed through the
// name CollisionType.
// TODO: #1
enum class CollisionType { PlayerAndWall, PlayerAndEnemy };
enum class HitType { PlayerAndEnemy };

struct CollisionPayload {
  uint32_t entity_a_id;
  uint32_t entity_b_id;
  CollisionType collision_type;
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
  HitType hit_type;
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
