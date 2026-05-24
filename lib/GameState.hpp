/* GameState.hpp
 * Authored by Liam Harvell
 * This holds the state of the game defines the entities and their components.
 */

// NOTE:========================================================================
// 1. An entity needs to have is_active must be true for CollisionSystem to see
// it as of 2026-04-24 23:37.
// TODO:========================================================================
// #1 Move Entities to their own dedicated header file.
// #2 Create a wrapper class around the entity vector in the class. Like
// EventQueue.
// #3 I need the entities origin to be in the logical center.
// #4 Tie the direction component to input or something.
// #6 I need to make a factory for entities.
// =============================================================================

#pragma once

#include "Bitmask.hpp"
#include "Util.hpp"
#include <EventQueue.hpp>
#include <cstdint>
#include <optional>
#include <sys/types.h>
#include <vector>

struct Bitmask {
  GameLayer layer;
  GameLayer mask;
};
// We can use an enum to define the possible shapes we can use and have
// deterministic outcomes on operating methods.
enum class ColliderShape { Rectangle, Circle };
// attach this component to the entity
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
  Vec2 move; // If we "normalize" this to -1.0:1.0 we can do fun
             // stuff with speed later I bet.
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

struct Sprite {
  std::string texture_key;
  float width, height;
};

enum class WallSide { Top, Bottom, Left, Right };
struct WallInfo {
  WallSide side;
};

// TODO: Swap out the Gun component for the Raycaster component.
struct Raycaster {
  float distance;
  float last_fired;
};

// WARN: Space-invaders structs, these make lib aware of space-invaders
// TODO: Make ShootingSystem to check for a cooldown.
struct Gun {
  float distance;
  bool fire_flag;
  Raycaster raycaster;
  float cooldown_time;
};

enum class AlienType { Squid, Ship, Crab, Octopus };

struct Alien {
  AlienType type;
};
struct AlienFormation {
  std::vector<Alien> aliens;
};

// FIX: Fix up this component
struct AlterMovement {
  std::optional<Vec2> position_update = Zero();
  std::optional<Vec2> speed_assignment = One();
  bool suppress_velocity = false;
};

// This is the E (Entity) in the ECS
struct Entity {
  uint32_t id;
  bool is_active;
  std::optional<Bitmask> bitmask;
  std::optional<PlayerInput> player_input;
  std::optional<Velocity> velocity;
  std::optional<Transform> transform;
  std::optional<Sprite> sprite;
  std::optional<Collider> collider;
  std::optional<WallInfo> wall_info;
  std::optional<Alien> alien_info;
  std::optional<AlterMovement> movement_mod;

  // WARN: This stuff shouldn't be here but I don't really have a fix right now.
  // space_invaders stuff
  std::optional<Gun> gun;
};

// FIXME: This should probably exist in the game mechanics source file
// but not in the class. That way they can see eachother, but aren't entirely
// coupled.
//-----------------------------------------------------------------------------
// using wrappedTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
// struct AssetManager {
//   void loadTexture(std::string path) {
//     SDL_Texture *raw_texture = IMG_LoadTexture(renderer, path.c_str());
//     textures[path] = wrappedTexture(raw_texture);
//   }
// };
//-----------------------------------------------------------------------------

class GameState {
private:
  uint32_t next_id = 0;

public:
  std::vector<Entity> entities;
  EventQueue event_queue;
  bool bullet_is_active = false;

  // This had to be here because the copy constructor below that
  // is deleted suppreses the compilers creation of any
  // constructor unless explicitly declared.
  GameState() = default;
  // make this class non-copyable
  // GameState(const GameState &) = delete;
  // GameState &operator=(const GameState &) = delete;

  Entity &CreateEntity();
  void DestroyEntity(uint32_t target_id);
};
