/* GameState.hpp
 * Authored by Liam Harvell
 * This holds the state of the game defines the entities and their components.
 */

#pragma once

#include <EventQueue.hpp>
#include <cstdint>
#include <optional>
#include <sys/types.h>
#include <vector>

// NOTE: I read that this is the most efficient way of attaching an attribute
// that may be needed by some other system. In this case I want the render
// system to know that the entity with this component is the player.
// Leaving it as an optional struct also lets us modify things later.
struct IsPlayer {};

// We can use an enum to define the possible shapes we can use and have
// deterministic outcomes on operating methods.
enum class ColliderShape { Rectangle, Circle };

// attach this component to the entity
struct Collider {
  ColliderShape shape;
  float offset_x, offset_y;

  // This is called an ANONYMOUS Union, only one of the structs can be valid at
  // the same time
  union {
    struct {
      float width, height;
    } rect;

    struct {
      float radius;
    } circle;
  };
};

// These are the C (Component) in an ECS
struct Velocity {
  float dx;
  float dy;
};

struct Transform {
  float x;
  float y;
};

// NOTE: I assume I'll need this later.
struct Direction {
  float angle;
};

struct Sprite {
  std::string texture_key;
  float width, height;
};

// This is the E (Entity) in the ECS
struct Entity {
  uint32_t id;
  bool is_active;
  std::optional<Velocity> velocity;
  std::optional<Transform> transform;
  std::optional<Sprite> sprite;
  std::optional<IsPlayer> is_player;
  std::optional<Direction> direction;
  std::optional<Collider> collider;
};

class GameState {
private:
  uint32_t next_id = 0;

public:
  std::vector<Entity> entities;
  EventQueue event_queue;

  // This had to be here because the copy constructor below that
  // is deleted suppreses the compilers creation of any
  // constructor unless explicitly declared.
  GameState() = default;
  // make this class non-copyable
  GameState(const GameState &) = delete;
  GameState &operator=(const GameState &) = delete;

  Entity &CreateEntity();
  void DestroyEntity(uint32_t target_id);
};
