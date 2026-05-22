#include "EntityFactory.hpp"
#include "GameState.hpp"
#include "Util.hpp"
// TODO: Implement EntityFactory - 2026-05-17

void EntityFactory::createPlayer() {

  Entity &player = game_state_.CreateEntity();
  player.bitmask.emplace(Bitmask{.layer = GameLayer::Player,
                                 .mask = GameLayer::Wall | GameLayer::Enemy});
  player.is_active = true;
  player.velocity.emplace(Velocity{.speed = 10.0f, .offset = Vec2{0.0f, 0.0f}});
  player.transform.emplace(
      Transform{Vec2{window_width_ / 2.0f, 7.0f * (window_height_ / 8.0f)},
                Vec2{0.0f, -1.0f}});
  player.collider.emplace(
      Collider{.shape = ColliderShape::Rectangle, .rect{100.0, 100.0}});
  player.player_input.emplace(
      PlayerInput{.move = Vec2{0.0f, 0.0f}, .is_firing = false});
  player.gun.emplace(Gun{.distance = 900.0, .fire_flag = false});
}

void EntityFactory::createGameWalls() {

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
  bottom_wall.transform.emplace(Transform{
      Vec2{window_width_ / 2.0f, (float)window_height_}, Vec2{0.0f, 0.0f}});
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
  right_wall.transform.emplace(Transform{
      Vec2{(float)window_width_, window_height_ / 2.0f}, Vec2{0.0f, 0.0f}});
  right_wall.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                       .offset_x = -5.0f,
                                       .rect{10.0f, (float)window_height_}});
}

void EntityFactory::createAlien(AlienType species, Vec2 position) {

  auto &alien = game_state_.CreateEntity();

  alien.bitmask.emplace(Bitmask{.layer = GameLayer::Enemy,
                                .mask = GameLayer::Player | GameLayer::Wall});

  alien.alien_info.emplace(Alien{.type = species});

  alien.velocity.emplace(
      Velocity{.speed = width_unit, .offset = Vec2{0.0f, 0.0f}});

  alien.transform.emplace(Transform{position, Vec2{0.0f, 0.0f}});

  alien.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                  .rect{2 * width_unit, 2 * height_unit}});

  alien.is_active = true;
}

void EntityFactory::createAlienFormation() {
  struct AlienFormationConfig {
    int rows = 5;
    int columns = 11;
    Vec2 start_pos = Vec2{.x = (3.0 * 1920.0) / 44.0, .y = (3.0 * 1080.0) / 44};
  } config;

  for (int i = 1; i <= config.rows; i++) {
    for (int j = 1; j <= config.columns; j++) {
      if (i == 1) {
        createAlien(AlienType::Squid,
                    Vec2{.x = i * 3 * height_unit + config.start_pos.x,
                         .y = j * 3 * width_unit + config.start_pos.y});
      } else if (i >= 2 && i < 5) {
        createAlien(AlienType::Crab,
                    Vec2{.x = i * 3 * height_unit + config.start_pos.x,
                         .y = j * 3 * width_unit + config.start_pos.y});
      } else {
        createAlien(AlienType::Crab,
                    Vec2{.x = i * 3 * height_unit + config.start_pos.x,
                         .y = j * 3 * width_unit + config.start_pos.y});
      }
    }
  }
}

// Constructor that uses an initalizer list because it's more efficient?
EntityFactory::EntityFactory(GameState &game_state, float window_width,
                             float window_height)
    : game_state_(game_state), window_width_(window_width),
      window_height_(window_height) {}
