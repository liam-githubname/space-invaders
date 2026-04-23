/* Events.hpp
 * Authored by Liam Harvell
 * I originally was housing this in the GameState file.
 * However, it had to be included by the CollisionSystem that needs to emit an
 * Event with a CollisionPayload. That meant that the CollisionSystem had to
 * include the GameState.
 * This is a better solution.
 */

// TODO: define a style for these payloads
// The "payloads", this is where events are defined. Events are
#pragma once

#include <cstdint>
#include <variant>

enum class CollisionType { PlayerAndWall, PlayerAndEnemy };

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

// TODO: Write doc for clangd to tell me how to add Event variants when I
// inevitably forget.
// NOTE: std::variant is a typesafe union. I was considering
// using optional fields like how entities work, but after looking into it these
// seem to be a good option. This type alias Event causes every event to be size
// of the largest payload. I think there might be a more memory efficient way of
// doing this with anonymous unions?
using Event = std::variant<CollisionPayload, DeathPayload, ScorePayload>;
