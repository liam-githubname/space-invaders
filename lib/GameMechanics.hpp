#pragma once

#include <cstdint>
#include <optional>
#include <vector>

struct SDL_Texture;

// NOTE: I read that this is the most efficient way of attaching an attribute
// that may be needed by some other system. In this case I want the render
// system to know that the entity with this component is the player.
// Leaving it as an optional struct also lets us modify things later.
struct isPlayer {};

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

struct Entity {
  uint32_t id;
  bool is_active;
  std::optional<Velocity> velocity;
  std::optional<Transform> transform;
  std::optional<Sprite> sprite;
  std::optional<isPlayer> isPlayer;
  std::optional<Direction> direction;
  std::optional<Collider> collider;
};

class GameState {
private:
  uint32_t next_id = 0;

public:
  std::vector<Entity> entities;

  // This had to be here because the copy constructor below that is deleted
  // suppreses the compilers creation of any constructor unless explicitly
  // declared.
  GameState() = default;
  // make this class non-copyable
  GameState(const GameState &) = delete;
  GameState &operator=(const GameState &) = delete;

  Entity &CreateEntity();
  void DestroyEntity(uint32_t target_id);
};

// You should not wrap your std::vector in a unique_ptr. You should keep it as a
// direct member variable of the GameWorld class.
//
// 2. Why it’s redundant (The "Double Heap" Problem)
// A std::vector is already a "manager" of memory.
//
// It stores a small amount of data on the Stack (pointers to its data, the
// size, and the capacity).
//
// It stores the actual Entity data on the Heap.
//
// If you wrap the vector in a unique_ptr, you are creating a "Pointer to a
// Manager of a Pointer."
//
// You have a pointer on the stack.
//
// It points to the std::vector object on the heap.
//
// The std::vector object points to the actual data somewhere else on the heap.
//
// This creates Double Indirection. Every time your systems want to move an
// entity, the CPU has to "jump" twice to find the data. In game programming,
// "jumps" are the enemy of performance.
// struct EntityVectorDeleter {
//   void operator()(std::vector<Entity> &entities) {
//     if (entities.capacity() > 0) {
//       for ()
//     }
//   }
// };
