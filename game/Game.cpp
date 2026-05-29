#include "Game.hpp"
#include "AssetManager.hpp"
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

Game::Game(GraphicsModule &&graphics, float window_width, float window_height)
    : graphics_(std::move(graphics)), input_system_(InputSystem::create()),
      game_state_(GameState()), is_running(true), window_width_(window_width),
      window_height_(window_height) {
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
  Game game(std::move(sdl_graphics_result.value()), (float)width,
            (float)height);

  return game;
  ;
}

struct config {
  float game_margins = 140.0f;
};

void Game::initializeGame() {

  // Initialize the assets first
  asset_manager_.Initialize(graphics_, game_state_);

  // create the factory
  auto factory =
      EntityFactory(game_state_, asset_manager_, window_width_, window_height_);

  // factory creates entites that use the assets
  factory.createGameWalls();
  factory.createPlayer();
  factory.createAlienFormation(Zero());
  factory.createMysteryShipSpawner();
}

void Game::run() {

  initializeGame();

  SDL_Event event;
  SDL_zero(event);

  // FIX: Either make this a private member of Game or figure out a better way
  // to create it.
  auto entity_factory =
      EntityFactory(game_state_, asset_manager_, window_width_, window_height_);

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
      shooting_system_.Update(game_state_, asset_manager_);
    }
    //========================== Consume Events =============================
    event_system_.ProcessEvents(game_state_);
    //========================================================================
    ui_system_.Update(graphics_, game_state_, asset_manager_);
    //=========================================================================
    mystery_ship_system_.Update(game_state_, asset_manager_);
    //==================================================================
    round_system_.Update(game_state_, render_system_, entity_factory,
                         graphics_.getRenderer());
    //============================ Render =====================================
    render_system_.Update(game_state_, graphics_.getRenderer());
  }
  return;
}
