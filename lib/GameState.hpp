/* GameState.hpp
 * Authored by Liam Harvell
 * This holds the state of the game defines the entities and their components.
 */
// NOTE:========================================================================
// 1. An entity needs to have is_active must be true for CollisionSystem to see
// it as of 2026-04-24 23:37.
// TODO:========================================================================
// #2 Create a wrapper class around the entity vector in the class. Like
// EventQueue.
// =============================================================================

#pragma once

#include "GameConfig.hpp"
#include "CoreComponents.hpp"
#include "AnimationComponents.hpp"
#include "AlienComponents.hpp"
#include <EventQueue.hpp>
#include <cstdint>
#include <optional>
#include <sys/types.h>
#include <vector>

/*
 * I wanted to implement an enitity component system for this game. I think that my instincts toward Object Orientation
 * kind of took over and I implemented the system backwards. So this format is far closer to a composition style.
 * For my next project, I would like to actually implement an ECS for all of it's benefits. For this game all of my
 * systems update by looking for only specific components through the main entities vector, instead of running through
 * sets of components. So I am not getting access to a lot of the performance benefits of an ECS like cache coherence,
 * nor could I make this game multithreaded very easily.
 *
 * This game wasn't going to really benefit from the performance gains of the proper implementation, so by the time I
 * realized my mistake it did not make sense to fix it.
 */

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
  std::optional<Gun> gun;
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
  int number_of_aliens = GameConfig::ALIENS_PER_ROUND;
  int total_number_of_aliens = GameConfig::ALIENS_PER_ROUND;
  int number_of_lives = GameConfig::STARTING_LIVES;

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
