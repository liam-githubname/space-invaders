#include "Game.hpp"
#include "CollisionSystem.hpp"
#include "EntityFactory.hpp"
#include "EventSystem.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include "Timestep.hpp"
#include "Util.hpp"
#include <expected>

Game::Game(GraphicsModule &&graphics)
    : graphics_(std::move(graphics)), input_system_(InputSystem::create()),
      game_state_(GameState()), is_running(true) {
  /*
   * Members with default constructors will auto-initialize.
   */
}

std::expected<Game, std::string> Game::create(std::string_view title, int width,
                                              int height) {

  // This is resource Acquisition plus the GraphicsModule object is wrapped
  // in an expected type.
  auto sdl_graphics_result = GraphicsModule::create(title, width, height);
  if (!sdl_graphics_result) {
    SDL_Log("SDL Graphics failed to start: %s",
            sdl_graphics_result.error().c_str());
    return std::unexpected(sdl_graphics_result.error());
  }

  auto input_system = InputSystem::create();
  // This unwraps the Graphics Module
  // The program needs to take ownership of the module
  // We have to use std::move because we removed the copy constructor.
  Game game(std::move(sdl_graphics_result.value()));

  return game;
  ;
}

void Game::initializeGame() {

  auto factory = EntityFactory(game_state_, window_width_, window_height_);

  auto width_unit = window_width_ / 44;
  auto height_unit = window_height_ / 44;

  // ==================== Initialization of entities
  // ==========================
  factory.createPlayer();
  factory.createGameWalls();
  factory.createAlienFormation();
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 3 * width_unit,
  //                                           .y = 3 * height_unit,
  //                                       });
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 3 * width_unit,
  //                                           .y = 6 * height_unit,
  //                                       });
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 3 * width_unit,
  //                                           .y = 9 * height_unit,
  //                                       });
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 3 * width_unit,
  //                                           .y = 12 * height_unit,
  //                                       });
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 6 * width_unit,
  //                                           .y = 15 * height_unit,
  //                                       });
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 6 * width_unit,
  //                                           .y = 18 * height_unit,
  //                                       });
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 6 * width_unit,
  //                                           .y = 21 * height_unit,
  //                                       });
  // factory.createAlien(AlienType::Squid, Vec2{
  //                                           .x = 6 * width_unit,
  //                                           .y = 24 * height_unit,
  //                                       });
}

void Game::run() {

  initializeGame();

  SDL_Event event;
  SDL_zero(event);

  while (is_running) {
    // It is important to realize that the input_system actually relies on
    // this call to SDL_PollEvent to update the keyboard state array
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        Game::is_running = false;
    }

    time_step_.Tick();

    while (time_step_.consumeStep()) {
      //========================== Input & Logic
      //==============================
      input_system_.Update(game_state_);
      //========================== Movement
      //===================================
      movement_system_.Update(game_state_, TimeStep::GetCurrentTime());
      //========================== Collision
      //==================================
      collision_system_.Update(game_state_);
      //========================== Shooting
      //===================================
      shooting_system_.Update(game_state_);
      //========================== Consume Events
      //=============================
      event_system_.ProcessEvents(game_state_);
    }
    //============================ Render
    //=====================================
    render_system_.Update(game_state_, graphics_.getRenderer());
  }
  return;
}
