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
#include "GraphicsModule.hpp"
#include "Util.hpp"
#include <EventQueue.hpp>
#include <cmath>
#include <cstdint>
#include <optional>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

struct Bitmask {
  GameLayer layer = GameLayer::None;
  GameLayer mask = GameLayer::None;
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

// TODO: Figure out a way to have not have both?
struct Ticker {
  int tick_count = 0;
  int max_ticks = 0;
};

struct Health {
  int max_hp = 5;
  int hp = max_hp;
};

struct MysteryTicker {
  int tick_count = 0;
};

// TODO: I'll make an enum for the sprites
// Space-invaders had a readily available sprite sheet, so instead of storing a
// key for sprite I'm storing a sprites coordinates on the sprite sheet.
struct SpriteData {
  SDL_FRect frame1;
  std::optional<SDL_FRect> frame2;
};
struct Sprite {
  SpriteData frame_data;
  bool step_1 = true;
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
  float distance = INFINITY;
  bool fire_flag;
  Raycaster raycaster;
  float cooldown_time;
};

struct TimedDeath {
  Ticker ticker;
};

struct ParentEntityClass {
  bool was_player;
};

enum class AlienType { Squid, Ship, Crab, Octopus };
// TODO: see if I can make the score dedicated to the AlienType
struct Alien {
  AlienType type;
  int score = 0;
  std::optional<Ticker> death_ticker;
};
struct AlienFormation {
  std::vector<Alien> aliens;
};

// FIX: Fix up this component
struct AlterMovement {
  std::optional<Vec2> position_update = Zero();
  std::optional<Vec2> speed_assignment = Zero();
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
  std::optional<Sprite> death_sprite;
  std::optional<Collider> collider;
  std::optional<WallInfo> wall_info;
  std::optional<Alien> alien_info;
  std::optional<AlterMovement> movement_mod;
  std::optional<MysteryTicker> mystery_ticker;
  std::optional<TimedDeath> time_death;
  std::optional<Health> health;
  std::optional<ParentEntityClass> parent;

  // WARN: This stuff shouldn't be here but I don't really have a fix right now.
  // space_invaders stuff
  std::optional<Gun> gun;
};

struct SpriteSheet {
  TexturePtr sprite_sheet;
  std::unordered_map<std::string, SDL_Rect> coordinate_map;
};

class GameState {
private:
  uint32_t next_id_ = 0;

public:
  std::vector<Entity> entities;
  EventQueue event_queue;
  bool bullet_is_active = false;
  SpriteSheet textures;
  int score = 0;
  bool score_update = false;
  bool lives_update = false;
  SDL_Texture *score_texture = nullptr;
  SDL_Texture *score_board_texture = nullptr;
  SDL_Texture *live_text_texture = nullptr;
  SDL_Texture *lives_texture = nullptr;
  SDL_Texture *menu_title_texture = nullptr;
  SDL_Texture *menu_prompt_texture = nullptr;
  SDL_Texture *game_over_title_texture = nullptr;
  SDL_Texture *game_over_prompt_texture = nullptr;
  int number_of_aliens = 55;
  int total_number_of_aliens = 55;
  int number_of_lives = 5;

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
