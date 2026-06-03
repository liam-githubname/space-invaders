#include "Game.hpp"
#include "AssetManager.hpp"
#include "CollisionSystem.hpp"
#include "EntityFactory.hpp"
#include "EventSystem.hpp"
#include "GameConfig.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include "Timestep.hpp"
#include "Util.hpp"
#include <expected>

Game::Game(GraphicsModule &&graphics, float window_width, float window_height)
    : graphics_(std::move(graphics)), input_system_(InputSystem::create()), game_state_(GameState()), is_running(true),
      window_width_(window_width), window_height_(window_height) {
  /*
   * Members with default constructors will auto-initialize.
   */
}

std::expected<Game, std::string> Game::create(std::string_view title, int width, int height) {
  // This is resource Acquisition plus the GraphicsModule object is wrapped
  // in an expected type.
  auto sdl_graphics_result = GraphicsModule::create(title, width, height);
  if (!sdl_graphics_result) {
    SDL_Log("SDL Graphics failed to start: %s", sdl_graphics_result.error().c_str());
    return std::unexpected(sdl_graphics_result.error());
  }

  auto input_system = InputSystem::create();
  // This unwraps the Graphics Module
  // The program needs to take ownership of the module
  // We have to use std::move because we removed the copy constructor.
  Game game(std::move(sdl_graphics_result.value()), (float)width, (float)height);

  return game;
  ;
}

void Game::initializeGame(EntityFactory &factory) {
  // Initialize the assets first
  asset_manager_.Initialize(graphics_, game_state_);

  // factory creates entites that use the assets
  factory.createGameWalls();
  factory.createBarriers();
  factory.createPlayer();
  factory.createAlienFormation(Zero());
  factory.createMysteryShipSpawner();
}

void Game::run_game_over() {
  asset_manager_.createGameOverTextures(graphics_, game_state_);
  while (game_over_running) {
    while (SDL_PollEvent(&event_)) {
      if (event_.type == SDL_EVENT_QUIT) {
        game_over_running = false;
      }
      if (event_.type == SDL_EVENT_KEY_DOWN) {
        if (event_.key.key == SDLK_SPACE) {
          game_over_running = false;
        }
      }
    }

    // clear the renderer and present start screen
    SDL_SetRenderDrawColor(graphics_.getRenderer(), 0, 0, 0, 255);
    SDL_RenderClear(graphics_.getRenderer());

    if (game_state_.game_over_title_texture != nullptr) {
      float title_w = (float)game_state_.game_over_title_texture->w * GameConfig::MENU_TITLE_SCALE;
      float title_h = (float)game_state_.game_over_title_texture->h * GameConfig::MENU_TITLE_SCALE;
      SDL_FRect title_rect{(window_width_ - title_w) / 2.0f, window_height_ * GameConfig::MENU_TITLE_FRAC_Y, title_w,
                           title_h};
      SDL_RenderTexture(graphics_.getRenderer(), game_state_.game_over_title_texture, NULL, &title_rect);
    }

    if (game_state_.game_over_prompt_texture != nullptr) {
      float prompt_w = (float)game_state_.game_over_prompt_texture->w * GameConfig::MENU_TITLE_SCALE;
      float prompt_h = (float)game_state_.game_over_prompt_texture->h * GameConfig::MENU_TITLE_SCALE;
      SDL_FRect prompt_rect{(window_width_ - prompt_w) / 2.0f, window_height_ * GameConfig::MENU_PROMPT_FRAC_Y,
                            prompt_w, prompt_h};
      SDL_RenderTexture(graphics_.getRenderer(), game_state_.game_over_prompt_texture, NULL, &prompt_rect);
    }

    SDL_RenderPresent(graphics_.getRenderer());
  }
  // clean up
  SDL_DestroyTexture(game_state_.game_over_title_texture);
  SDL_DestroyTexture(game_state_.game_over_prompt_texture);
  game_state_.game_over_title_texture = nullptr;
  game_state_.game_over_prompt_texture = nullptr;
}

void Game::run_menu() {
  asset_manager_.loadFont();
  asset_manager_.createMenuTextures(graphics_, game_state_);

  // Keep player in the menu while they havent' pressed space
  while (start_menu_running) {
    while (SDL_PollEvent(&event_)) {
      if (event_.type == SDL_EVENT_QUIT) {
        start_menu_running = false;
        is_running = false;
      }
      if (event_.type == SDL_EVENT_KEY_DOWN) {
        if (event_.key.key == SDLK_SPACE) {
          start_menu_running = false;
        }
      }
    }

    // clear the renderer and present start screen
    SDL_SetRenderDrawColor(graphics_.getRenderer(), 0, 0, 0, 255);
    SDL_RenderClear(graphics_.getRenderer());

    if (game_state_.menu_title_texture != nullptr) {
      float title_w = (float)game_state_.menu_title_texture->w * GameConfig::MENU_TITLE_SCALE;
      float title_h = (float)game_state_.menu_title_texture->h * GameConfig::MENU_TITLE_SCALE;
      SDL_FRect title_rect{(window_width_ - title_w) / 2.0f, window_height_ * GameConfig::MENU_TITLE_FRAC_Y, title_w,
                           title_h};
      SDL_RenderTexture(graphics_.getRenderer(), game_state_.menu_title_texture, NULL, &title_rect);
    }

    if (game_state_.menu_prompt_texture != nullptr) {
      float prompt_w = (float)game_state_.menu_prompt_texture->w * GameConfig::MENU_TITLE_SCALE;
      float prompt_h = (float)game_state_.menu_prompt_texture->h * GameConfig::MENU_TITLE_SCALE;
      SDL_FRect prompt_rect{(window_width_ - prompt_w) / 2.0f, window_height_ * GameConfig::MENU_PROMPT_FRAC_Y,
                            prompt_w, prompt_h};
      SDL_RenderTexture(graphics_.getRenderer(), game_state_.menu_prompt_texture, NULL, &prompt_rect);
    }

    SDL_RenderPresent(graphics_.getRenderer());
  }

  // clean up
  SDL_DestroyTexture(game_state_.menu_title_texture);
  SDL_DestroyTexture(game_state_.menu_prompt_texture);
  game_state_.menu_title_texture = nullptr;
  game_state_.menu_prompt_texture = nullptr;
}

void Game::run() {
  run_menu();

  // FIX: Either make this a private member of Game or figure out a better
  // way to create it.
  auto entity_factory = EntityFactory(game_state_, asset_manager_, window_width_, window_height_);

  initializeGame(entity_factory);

  SDL_Event event;
  SDL_zero(event);

  while (is_running) {
    // It is important to realize that the input_system actually relies on
    // this call to SDL_PollEvent to update the keyboard state array
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        Game::is_running = false;
        Game::game_over_running = false;
      }
    }

    time_step_.Tick();

    while (time_step_.consumeStep()) {
      input_system_.Update(game_state_);
      movement_system_.Update(game_state_, TimeStep::GetCurrentTime());
      collision_system_.Update(game_state_);
      shooting_system_.Update(game_state_, asset_manager_);
    }
    ui_system_.Update(graphics_, game_state_, asset_manager_);
    round_system_.Update(game_state_, render_system_, entity_factory, asset_manager_, graphics_.getRenderer(),
                         is_running);
    event_system_.ProcessEvents(game_state_);
    render_system_.Update(game_state_, graphics_.getRenderer());
  }

  run_game_over();

  return;
}
