#pragma once
#include "Util.hpp"
#include "Bitmask.hpp"
#include <cmath>

enum class ColliderShape { Rectangle, Circle };
struct Collider {
  ColliderShape shape;
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

struct PlayerInput {
  Vec2 move;
  bool is_firing;
};

// These are the C (Component) in an ECS
struct Velocity {
  Vec2 speed = Vec2{.x = 0.0f, .y = 0.0f};
};

struct Transform {
  Vec2 position;
  Vec2 direction;
};

struct AlterMovement {
  std::optional<Vec2> position_update = Zero();
  std::optional<Vec2> speed_assignment = Zero();
  bool suppress_velocity = false;
};

struct Ticker {
  int tick_count = 0;
  int max_ticks = 0;
};

struct Gun {
  float distance = INFINITY;
  bool fire_flag = false;
};

struct Health {
  int max_hp = 5;
  int hp = max_hp;
};

struct Bitmask {
  GameLayer layer = GameLayer::None;
  GameLayer mask = GameLayer::None;
};

enum class WallSide { Top, Bottom, Left, Right };
struct WallInfo {
  WallSide side;
};
struct ParentEntityClass {
  bool was_player;
};
struct TimedDeath {
  Ticker ticker;
};
