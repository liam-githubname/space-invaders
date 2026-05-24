#include "EntityFactory.hpp"
#include "Bitmask.hpp"
#include "GameState.hpp"
#include "Util.hpp"
// TODO: Implement EntityFactory - 2026-05-17

void EntityFactory::createPlayer() {
  // FIX: spaghetti code nightmare fix later

  Entity &player = game_state_.CreateEntity();
  player.bitmask.emplace(Bitmask{
      .layer = GameLayer::Player,
      .mask = GameLayer::Wall | GameLayer::Enemy,
  });
  player.is_active = true;
  player.velocity.emplace(Velocity{.speed = {2.0f, 0.0f}});
  player.transform.emplace(Transform{
      config.player_spawn_position,
      Up(),
  });
  player.collider.emplace(
      Collider{.shape = ColliderShape::Rectangle, .rect{12.0f, 6.0f}});
  player.player_input.emplace(PlayerInput{.move = Zero(), .is_firing = false});
  player.gun.emplace(
      Gun{.distance = window_height_ - 1.0f, .fire_flag = false});
}

void EntityFactory::createGameWalls() {
  // FIX: spaghetti code nightmare fix later

  float left_right_wall_offset = 1.5f;

  auto &top_wall = game_state_.CreateEntity();
  top_wall.bitmask.emplace(Bitmask{
      .layer = GameLayer::Wall,
      .mask =
          GameLayer::Player | GameLayer::Projectile, // I am adding projectiles
                                                     // to the top walls mask
  });
  top_wall.wall_info.emplace(WallSide::Top);
  top_wall.transform.emplace(Transform{
      Vec2{window_width_ / 2.0f, 0.0f},
      Vec2{0.0f, 0.0f},
  });

  top_wall.collider.emplace(Collider{
      .shape = ColliderShape::Rectangle,
      .rect{(float)window_width_, 0.01f},
  });

  // Bottom wall
  auto &bottom_wall = game_state_.CreateEntity();
  bottom_wall.bitmask.emplace(Bitmask{
      .layer = GameLayer::Wall,
      .mask = GameLayer::Player,
  });
  bottom_wall.wall_info.emplace(WallSide::Bottom);
  bottom_wall.is_active = true;
  bottom_wall.transform.emplace(Transform{
      Vec2{window_width_ / 2.0f, (float)window_height_ - 0.5f},
      Vec2{0.0f, 0.0f},
  });
  bottom_wall.collider.emplace(Collider{
      .shape = ColliderShape::Rectangle,
      .rect{(float)window_width_, 1.0f},
  });
  // Left wall
  auto &left_wall = game_state_.CreateEntity();
  left_wall.bitmask.emplace(
      Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Player});
  left_wall.wall_info.emplace(WallSide::Left);
  left_wall.is_active = true;
  left_wall.transform.emplace(Transform{
      Vec2{left_right_wall_offset, window_height_ / 2.0f},
      Vec2{0.0f, 0.0f},
  });
  left_wall.collider.emplace(Collider{
      .shape = ColliderShape::Rectangle,
      .rect{1.0f, (float)window_height_},
  });
  // Right wall
  auto &right_wall = game_state_.CreateEntity();
  right_wall.bitmask.emplace(Bitmask{
      .layer = GameLayer::Wall,
      .mask = GameLayer::Player,
  });
  right_wall.wall_info.emplace(WallSide::Right);
  right_wall.is_active = true;
  right_wall.transform.emplace(Transform{
      Vec2{(float)window_width_ - left_right_wall_offset,
           window_height_ / 2.0f},
      Vec2{0.0f, 0.0f},
  });
  right_wall.collider.emplace(Collider{
      .shape = ColliderShape::Rectangle,
      .rect{1.0f, (float)window_height_},
  });
}

void EntityFactory::createAlien(AlienType species, Vec2 position) {

  // FIX: spaghetti code nightmare fix later

  auto &alien = game_state_.CreateEntity();

  alien.bitmask.emplace(Bitmask{.layer = GameLayer::Enemy,
                                .mask = GameLayer::Player | GameLayer::Wall |
                                        GameLayer::Projectile});

  alien.alien_info.emplace(Alien{.type = species});

  alien.velocity.emplace(Velocity{.speed = {2.0f}});

  alien.transform.emplace(Transform{position, Vec2{0.0f, 0.0f}});

  alien.collider.emplace(
      Collider{.shape = ColliderShape::Rectangle, .rect{8.0f, 3.5f}});

  alien.is_active = true;
}

void EntityFactory::createAlienFormation() {

  for (int i = 1; i <= config.rows; i++) {
    for (int j = 1; j <= config.columns; j++) {
      if (i == 1) {
        createAlien(AlienType::Squid,
                    Vec2{.x = i * 18 + config.first_alien_position.x,
                         .y = j * 9 + config.first_alien_position.y});
      } else if (i >= 2 && i < 5) {
        createAlien(AlienType::Crab,
                    Vec2{.x = i * 18 + config.first_alien_position.x,
                         .y = j * 9 + config.first_alien_position.y});
      } else {
        createAlien(AlienType::Crab,
                    Vec2{.x = i * 18 + config.first_alien_position.x,
                         .y = j * 9 + config.first_alien_position.y});
      }
    }
  }
}

// Constructor that uses an initalizer list because it's more efficient, it
// doesn't create temporary memory to copy over, because it's not code that runs
// in the constructor, it's the allocation of the struct itself.
EntityFactory::EntityFactory(GameState &game_state, float window_width,
                             float window_height)
    : game_state_(game_state), window_width_(window_width),
      window_height_(window_height) {}
