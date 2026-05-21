#include "Game.hpp"
#include "CollisionSystem.hpp"
#include "EventSystem.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include "Timestep.hpp"
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

  // ==================== Initialization of entities ==========================
  // TODO: Look at making CreateEntity a factory pattern?
  Entity &player = game_state_.CreateEntity();
  player.bitmask.emplace(Bitmask{.layer = GameLayer::Player,
                                 .mask = GameLayer::Wall | GameLayer::Enemy});
  player.is_active = true;
  player.velocity.emplace(
      Velocity{.speed = 10.0f, .offset = Vec2{0.0f, 0.0f}});
  player.transform.emplace(
      Transform{Vec2{window_width_ / 2.0f, 7.0f * (window_height_ / 8.0f)},
                Vec2{0.0f, -1.0f}});
  player.collider.emplace(
      Collider{.shape = ColliderShape::Rectangle, .rect{100.0, 100.0}});
  player.player_input.emplace(
      PlayerInput{.move = Vec2{0.0f, 0.0f}, .is_firing = false});
  player.gun.emplace(Gun{.distance = 900.0, .fire_flag = false});

  auto &top_wall = game_state_.CreateEntity();
  top_wall.bitmask.emplace(
      Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Player});
  top_wall.wall_info.emplace(WallSide::Top);
  top_wall.transform.emplace(
      Transform{Vec2{window_width_ / 2.0f, 0.0f}, Vec2{0.0f, 0.0f}});
  top_wall.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                     .offset_y = 5.0f,
                                     .rect{(float)window_width_, 10.0f}});
  // Bottom wall
  auto &bottom_wall = game_state_.CreateEntity();
  bottom_wall.bitmask.emplace(
      Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Player});
  bottom_wall.wall_info.emplace(WallSide::Bottom);
  bottom_wall.is_active = true;
  bottom_wall.transform.emplace(
      Transform{Vec2{window_width_ / 2.0f, (float)window_height_},
                Vec2{0.0f, 0.0f}});
  bottom_wall.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                        .offset_y = 5.0,
                                        .rect{(float)window_width_, 10.0f}});
  // Left wall
  auto &left_wall = game_state_.CreateEntity();
  left_wall.bitmask.emplace(
      Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Player});
  left_wall.wall_info.emplace(WallSide::Left);
  left_wall.is_active = true;
  left_wall.transform.emplace(
      Transform{Vec2{0.0f, window_height_ / 2.0f}, Vec2{0.0f, 0.0f}});
  left_wall.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                      .offset_x = 5.0f,
                                      .rect{10.0f, (float)window_height_}});
  // Right wall
  auto &right_wall = game_state_.CreateEntity();
  right_wall.bitmask.emplace(
      Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Player});
  right_wall.wall_info.emplace(WallSide::Right);
  right_wall.is_active = true;
  right_wall.transform.emplace(
      Transform{Vec2{(float)window_width_, window_height_ / 2.0f},
                Vec2{0.0f, 0.0f}});
  right_wall.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                       .offset_x = -5.0f,
                                       .rect{10.0f, (float)window_height_}});

  auto &enemy = game_state_.CreateEntity();
  enemy.bitmask.emplace(Bitmask{.layer = GameLayer::Enemy,
                                .mask = GameLayer::Player | GameLayer::Wall});
  enemy.alien_info.emplace(Alien{.type = AlienSpecies::Squid});
  enemy.velocity.emplace(
      Velocity{.speed = 50.0, .offset = Vec2{0.0f, 0.0f}});
  enemy.transform.emplace(
      Transform{Vec2{window_width_ / 2.0f, window_height_ / 4.0f},
                Vec2{0.0f, 0.0f}});
  enemy.collider.emplace(
      Collider{.shape = ColliderShape::Rectangle, .rect{100.0, 100.0}});
  enemy.is_active = true;

  auto &enemy2 = game_state_.CreateEntity();
  enemy2.bitmask.emplace(Bitmask{.layer = GameLayer::Enemy,
                                 .mask = GameLayer::Player | GameLayer::Wall});
  enemy2.alien_info.emplace(Alien{.type = AlienSpecies::Squid});
  enemy2.velocity.emplace(
      Velocity{.speed = 50.0, .offset = Vec2{0.0f, 0.0f}});
  enemy2.transform.emplace(
      Transform{Vec2{window_width_ / 4.0f, window_height_ / 4.0f},
                Vec2{0.0f, 0.0f}});
  enemy2.collider.emplace(
      Collider{.shape = ColliderShape::Rectangle, .rect{100.0, 100.0}});
  enemy2.is_active = true;
  auto &enemy3 = game_state_.CreateEntity();
  enemy3.bitmask.emplace(Bitmask{.layer = GameLayer::Enemy,
                                 .mask = GameLayer::Player | GameLayer::Wall});
  enemy3.alien_info.emplace(Alien{.type = AlienSpecies::Squid});
  enemy3.velocity.emplace(
      Velocity{.speed = 50.0, .offset = Vec2{0.0f, 0.0f}});
  enemy3.transform.emplace(
      Transform{Vec2{window_width_ / 4.0f, window_height_ / 2.0f},
                Vec2{0.0f, 0.0f}});
  enemy3.collider.emplace(
      Collider{.shape = ColliderShape::Rectangle, .rect{100.0, 100.0}});
  enemy3.is_active = true;
}

// void Game::PassVisitorHandlersToEventSystem() {
//   event_system_.ProcessEvents(
//       game_state_,
//       Overload{
//           [&](const CollisionPayload &payload) {
//             game_rules_.HandleCollisionPayload(payload, game_state_);
//           },
//           [&](const DeathPayload &payload) {
//             game_rules_.HandleDeathPayload();
//           },
//           [&](const ScorePayload &payload) {
//             game_rules_.HandleScorePayload();
//           },
//           [&](const HitPayload &payload) { game_rules_.HandleHitPayload();
//           }});
// }

void Game::run() {

  initializeGame();
  SDL_Event event;
  SDL_zero(event);

  while (is_running) {
    // It is important to realize that the input_system actually relies on this
    // call to SDL_PollEvent to update the keyboard state array
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        Game::is_running = false;
    }

    time_step_.Tick();

    while (time_step_.consumeStep()) {
      //========================== Input & Logic ==============================
      input_system_.Update(game_state_);
      //========================== Movement ===================================
      movement_system_.Update(game_state_, TimeStep::GetCurrentTime());
      //========================== Collision ==================================
      collision_system_.Update(game_state_);
      //========================== Shooting ===================================
      shooting_system_.Update(game_state_);
      //========================== Consume Events =============================
      event_system_.ProcessEvents(game_state_);
    }
    //============================ Render =====================================
    render_system_.Update(game_state_, graphics_.getRenderer());
  }
  return;
}
