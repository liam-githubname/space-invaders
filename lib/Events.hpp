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

class GameState;
struct Entity;
// INFO: The reason I have class here is the enum must be accessed through the
// name CollisionType.
struct Event {
  virtual ~Event() = default;
  virtual void execute(GameState &game_state) const = 0;
};

struct CollisionEvent : public Event {
public:
  explicit CollisionEvent(uint32_t entity_a_id, uint32_t entity_b_id);
  virtual void execute(GameState &game_state) const override final;
  // CollisionEvent* make() {new CollisionEvent() };

private:
  uint32_t entity_a_id;
  uint32_t entity_b_id;
  void BulletCollisionHandler(Entity &entity_a, Entity &entity_b) const;
  void WallCollisionHandler(Entity &entity_a, Entity &entity_b, GameState &game_state) const;
};
