#pragma once

#include <cstdint>
#include <optional>
#include <vector>

struct Velocity {

  float dx;
  float dy;
};

struct Transform {
  float x;
  float y;
};

struct Sprite {
  void *texture;
  float width, height;
};

struct Entity {
  uint32_t id;
  bool is_active;
  std::optional<Velocity> velocity;
  std::optional<Transform> transform;
  std::optional<Sprite> sprite;
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
