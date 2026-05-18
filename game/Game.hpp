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
#include "SpaceInvadersMovementSystem.hpp"
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
  // Private Constructor
  // A constructor will quietly call it's members default constructors if they
  // have one. If not the Compiler will through an error. Game cannot have a
  // default constructor because it has been implicitly deleted because
  // GraphicsModules's constructor was deleted. (it was deleted because it's
  // copy semantics we're deleted which implicitly delets the default
  // constructor of a class)
  Game(GraphicsModule &&graphics);

  // Engine systems
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
  void initializeLibrarySystems(std::string_view title);

  // Miscellaneous
  bool is_running;
  float dt = 1.0f / 60.0f;
  float window_width_ = 1920;
  float window_height_ = 1080;

  // space_invaders specific
  // This is a virtual wrapper around EventSystem::ProcessEvents
  // Which takes a Visitor&& visitor parameter
  void PassVisitorHandlersToEventSystem();
  void initializeGame();

  SpaceInvadersMovementSystem gameplay_movement_;
};
