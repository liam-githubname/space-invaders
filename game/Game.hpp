#pragma once
// TODO: I need to figure out how to properly tell the compiler what all of
// these are

#include "CollisionSystem.hpp"
#include "EventSystem.hpp"
#include "GameRules.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include "RenderSystem.hpp"
#include "ShootingSystem.hpp"
#include "Timestep.hpp"

// I think I want to do the same thing that I did with GraphicsModule
class Game {
public:
  static std::expected<Game, std::string> create(std::string_view title,
                                                 int width, int height);
  void run();
  // Copy semantic is being deleted
  Game(const Game &) = delete;
  Game &operator=(const Game &) = delete;

  // The move-assignment operator cannot be marked default while there is a
  // const member. Because the const member implicitly deletes the move
  // assignment
  Game(Game &&) = default;
  Game &operator=(Game &&) = default;

private:
  TimeStep time_step_;
  GraphicsModule graphics_;
  GameState game_state_;
  SDL_Event event_;
  InputSystem input_system_;
  MovementSystem movement_system_;
  CollisionSystem collision_system_;
  EventSystem event_system_;
  RenderSystem render_system_;
  ShootingSystem shooting_system_;
  GameRules game_rules_;
  bool is_running;
  float dt = 1.0f / 60.0f;
  float window_width_ = 1920;
  float window_height_ = 1080;
  // This is a virtual wrapper around EventSystem::ProcessEvents
  // Which takes a Visitor&& visitor parameter
  void processGameEvents();
  void initializeLibrarySystems(std::string_view title);
  void initializeGame();

  Game(GraphicsModule &&graphics);
};
