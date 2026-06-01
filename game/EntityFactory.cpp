#include "EntityFactory.hpp"
#include "AssetManager.hpp"
#include "Bitmask.hpp"
#include "GameState.hpp"
#include "Util.hpp"
#include <optional>
// TODO: Implement EntityFactory - 2026-05-17

void EntityFactory::createPlayer() {
  // FIX: spaghetti code nightmare fix later

  auto player_movement_speed = Vec2{1.0f, 0.0f};

  Entity &player = game_state_.CreateEntity();
  player.bitmask.emplace(Bitmask{
      .layer = GameLayer::Player,
      .mask = GameLayer::Wall | GameLayer::Enemy,
  });
  player.health.emplace(Health{.max_hp = 3});
  player.is_active = true;
  player.velocity.emplace(Velocity{.speed = player_movement_speed});
  player.transform.emplace(Transform{
      config.player_spawn_position,
      Up(),
  });
  player.sprite.emplace(Sprite{.frame_data = asset_manager.textures["canon"]});
  player.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                   .rect{player.sprite->frame_data.frame1.w,
                                         player.sprite->frame_data.frame1.h}});
  player.player_input.emplace(PlayerInput{.move = Zero(), .is_firing = false});
}

Entity &create_barrier_part_by_key(GameState &game_state_,
                                   AssetManager &asset_manager,
                                   std::string asset_key) {

  auto &barrier_part = game_state_.CreateEntity();

  barrier_part.is_active = true;
  barrier_part.sprite.emplace(
      Sprite{.frame_data = asset_manager.textures[asset_key]});

  // Added a health component
  barrier_part.health.emplace(Health{.max_hp = 4});

  barrier_part.bitmask.emplace(
      Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Projectile});

  auto collider_width = barrier_part.sprite->frame_data.frame1.w;
  auto collider_height = barrier_part.sprite->frame_data.frame1.h;

  barrier_part.collider.emplace(
      Collider{.rect{collider_width, collider_height}});

  return barrier_part;
};

void EntityFactory::createBarrier(Vec2 position) {
  auto &barrier_part1 =
      create_barrier_part_by_key(game_state_, asset_manager, "barrier1");
  barrier_part1.transform.emplace(Transform{.position = position});

  auto &barrier_part2 =
      create_barrier_part_by_key(game_state_, asset_manager, "barrier2");
  barrier_part2.transform.emplace(
      Transform{.position.x = barrier_part1.transform->position.x,
                .position.y = barrier_part1.transform->position.y +
                              barrier_part1.collider->rect.height});

  auto &barrier_part3 =
      create_barrier_part_by_key(game_state_, asset_manager, "barrier3");
  barrier_part3.transform.emplace(
      Transform{.position.x = barrier_part1.transform->position.x +
                              barrier_part1.collider->rect.width,
                .position.y = barrier_part1.transform->position.y});

  auto &barrier_part4 =
      create_barrier_part_by_key(game_state_, asset_manager, "barrier4");
  barrier_part4.transform.emplace(Transform{
      .position.x = barrier_part3.transform->position.x,
      .position.y = barrier_part3.transform->position.y +
                    barrier_part3.collider->rect.height - 4.0f,
  });
  auto &barrier_part5 =
      create_barrier_part_by_key(game_state_, asset_manager, "barrier5");
  barrier_part5.transform.emplace(Transform{
      .position.x = barrier_part3.transform->position.x +
                    barrier_part3.collider->rect.width,
      .position.y = barrier_part3.transform->position.y,
  });
  auto &barrier_part6 =
      create_barrier_part_by_key(game_state_, asset_manager, "barrier6");
  barrier_part6.transform.emplace(Transform{
      .position.x = barrier_part5.transform->position.x,
      .position.y = barrier_part5.transform->position.y +
                    barrier_part5.collider->rect.height,
  });
}

void EntityFactory::createGameWalls() {
  // FIX: spaghetti code nightmare fix later

  float left_right_wall_offset = 1.5f;
  float wall_size = 1.0f;

  auto &top_wall = game_state_.CreateEntity();
  top_wall.is_active = true;
  top_wall.bitmask.emplace(Bitmask{
      .layer = GameLayer::Wall,
      .mask =
          GameLayer::Player | GameLayer::Projectile, // I am adding projectiles
                                                     // to the top wall's mask
  });
  top_wall.wall_info.emplace(WallSide::Top);
  top_wall.transform.emplace(Transform{Vec2{window_width_ / 2, 0.0f}, Zero()});

  top_wall.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                     .rect{(float)window_width_, wall_size}});

  // Bottom wall
  auto &bottom_wall = game_state_.CreateEntity();
  bottom_wall.bitmask.emplace(Bitmask{
      .layer = GameLayer::Wall,
      .mask = GameLayer::Player | GameLayer::Projectile,
  });
  bottom_wall.wall_info.emplace(WallSide::Bottom);
  bottom_wall.is_active = true;
  bottom_wall.transform.emplace(Transform{
      Vec2{window_width_ / 2, (float)window_height_ - 0.5f},
      Zero(),
  });
  bottom_wall.collider.emplace(Collider{
      .shape = ColliderShape::Rectangle,
      .rect{(float)window_width_, wall_size},
  });
  // Left wall
  auto &left_wall = game_state_.CreateEntity();
  left_wall.bitmask.emplace(
      Bitmask{.layer = GameLayer::Wall,
              .mask = GameLayer::Player | GameLayer::Projectile});
  left_wall.wall_info.emplace(WallSide::Left);
  left_wall.is_active = true;
  left_wall.transform.emplace(Transform{
      Vec2{left_right_wall_offset, window_height_ / 2},
      Zero(),
  });
  left_wall.collider.emplace(Collider{
      .shape = ColliderShape::Rectangle,
      .rect{wall_size, (float)window_height_},
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
      Zero(),
  });
  right_wall.collider.emplace(Collider{
      .shape = ColliderShape::Rectangle,
      .rect{wall_size, (float)window_height_},
  });
}

void EntityFactory::createAlien(AlienType species, Vec2 position) {

  auto species_to_key = [](AlienType species) {
    switch (species) {
    case AlienType::Squid:
      return "squid";
    case AlienType::Crab:
      return "crab";
    case AlienType::Octopus:
      return "octopus";
    case AlienType::Ship:
      return "spaceship";
    default:
      return "Not a species";
    }
  };
  auto species_to_score = [](AlienType species) {
    switch (species) {
    case AlienType::Squid:
      return 30;
    case AlienType::Crab:
      return 20;
    case AlienType::Octopus:
      return 10;
    case AlienType::Ship:
      return 150;
    default:
      return 0;
    }
  };

  // FIX: spaghetti code nightmare fix later

  auto &alien = game_state_.CreateEntity();

  alien.bitmask.emplace(Bitmask{.layer = GameLayer::Enemy,
                                .mask = GameLayer::Player | GameLayer::Wall |
                                        GameLayer::Projectile});

  alien.alien_info.emplace(
      Alien{.type = species, .score = species_to_score(species)});

  // TODO: remove magic numbers
  alien.velocity.emplace(Velocity{.speed = {2.0f}});

  // WARN: experimental health component;
  alien.health.emplace(Health{.max_hp = 1});

  alien.transform.emplace(Transform{position, Down()});

  alien.sprite.emplace(
      Sprite{.frame_data = asset_manager.textures[species_to_key(species)]});

  // Because it's cheap to hold just the explosion sprite in the entity, I'm
  // going to do that.
  alien.death_sprite.emplace(
      Sprite{.frame_data = asset_manager.textures["explosion"]});

  auto sprite_width = alien.sprite->frame_data.frame1.w;
  auto sprite_height = alien.sprite->frame_data.frame1.h;

  alien.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                  .rect{sprite_width, sprite_height}});

  alien.gun.emplace(Gun{.fire_flag = false});

  alien.is_active = true;
}

void EntityFactory::createAlienFormation(Vec2 position_update) {
  // Authentic Space Invaders layout: 5 rows x 11 columns
  // Horizontal spacing: 16px (gives ~4-8px gap depending on sprite width)
  // Vertical spacing: 15px (8px sprite + 7px gap as requested)
  // Row 0: Squid (top)
  // Rows 1-2: Crab (middle)
  // Rows 3-4: Octopus (bottom)

  config.first_alien_position += position_update;

  for (int row = 0; row < config.rows; row++) {
    for (int col = 0; col < config.columns; col++) {
      AlienType species;
      switch (row) {
      case 0:
        species = AlienType::Squid;
        break;
      case 1:
      case 2:
        species = AlienType::Crab;
        break;
      default:
        species = AlienType::Octopus;
        break;
      }

      float x = config.first_alien_position.x + col * 16.0f;
      float y = config.first_alien_position.y + row * 15.0f;

      createAlien(species, Vec2{x, y});
    }
  }
}

void EntityFactory::createMysteryShipSpawner() {
  auto &spawner = game_state_.CreateEntity();
  spawner.mystery_ticker.emplace();
  spawner.bitmask.emplace();
}

// Constructor that uses an initalizer list because it's more efficient, it
// doesn't create temporary memory to copy over, because it's not code that runs
// in the constructor, it's the allocation of the struct itself.
EntityFactory::EntityFactory(GameState &game_state, AssetManager &asset_manager,
                             float window_width, float window_height)
    : game_state_(game_state), asset_manager(asset_manager),
      window_width_(window_width), window_height_(window_height) {}
