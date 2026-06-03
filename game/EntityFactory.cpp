#include "EntityFactory.hpp"
#include "AssetManager.hpp"
#include "Bitmask.hpp"
#include "GameConfig.hpp"
#include "GameState.hpp"
#include "Util.hpp"
#include <optional>
// TODO: Implement EntityFactory - 2026-05-17

void EntityFactory::createPlayer() {
  // FIX: spaghetti code nightmare fix later

  Entity &player = game_state_.CreateEntity();
  player.is_active = true;

  player.bitmask.emplace(Bitmask{
    .layer = GameLayer::Player,
    .mask = GameLayer::Wall | GameLayer::Enemy,
  });
  player.health.emplace(Health{GameConfig::PLAYER_MAX_HP});
  player.velocity.emplace(GameConfig::PLAYER_SPEED);
  player.transform.emplace(Transform{
    GameConfig::PLAYER_SPAWN,
    GameConfig::PLAYER_DIRECTION,
  });

  player.sprite.emplace(Sprite{.frame_data = asset_manager.textures.at("canon")});

  player.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                   .rect{player.sprite->frame_data.frame1.w, player.sprite->frame_data.frame1.h}});

  player.player_input.emplace(PlayerInput{.move = Zero(), .is_firing = false});
}

Entity &create_barrier_part_by_key(GameState &game_state_, const AssetManager &asset_manager, std::string asset_key) {
  Entity &barrier_part = game_state_.CreateEntity();

  barrier_part.is_active = true;
  barrier_part.sprite.emplace(Sprite{.frame_data = asset_manager.textures.at(asset_key)});

  // Added a health component
  barrier_part.health.emplace(Health{.max_hp = GameConfig::BARRIER_PART_MAX_HP});

  barrier_part.bitmask.emplace(Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Projectile});

  const float collider_width = barrier_part.sprite->frame_data.frame1.w;
  const float collider_height = barrier_part.sprite->frame_data.frame1.h;

  barrier_part.collider.emplace(Collider{.rect{collider_width, collider_height}});

  return barrier_part;
};

void EntityFactory::createBarrier(const Vec2 position) {
  Entity &barrier_part1 = create_barrier_part_by_key(game_state_, asset_manager, "barrier1");
  barrier_part1.transform.emplace(Transform{.position = position});

  Entity &barrier_part2 = create_barrier_part_by_key(game_state_, asset_manager, "barrier2");
  barrier_part2.transform.emplace(
    Transform{.position.x = barrier_part1.transform->position.x,
              .position.y = barrier_part1.transform->position.y + barrier_part1.collider->rect.height});

  Entity &barrier_part3 = create_barrier_part_by_key(game_state_, asset_manager, "barrier3");
  barrier_part3.transform.emplace(
    Transform{.position.x = barrier_part1.transform->position.x + barrier_part1.collider->rect.width,
              .position.y = barrier_part1.transform->position.y});

  Entity &barrier_part4 = create_barrier_part_by_key(game_state_, asset_manager, "barrier4");
  barrier_part4.transform.emplace(Transform{
    .position.x = barrier_part3.transform->position.x,
    .position.y =
      barrier_part3.transform->position.y + barrier_part3.collider->rect.height - GameConfig::BARRIER_PART4_OFFSET_Y,
  });
  Entity &barrier_part5 = create_barrier_part_by_key(game_state_, asset_manager, "barrier5");
  barrier_part5.transform.emplace(Transform{
    .position.x = barrier_part3.transform->position.x + barrier_part3.collider->rect.width,
    .position.y = barrier_part3.transform->position.y,
  });
  Entity &barrier_part6 = create_barrier_part_by_key(game_state_, asset_manager, "barrier6");
  barrier_part6.transform.emplace(Transform{
    .position.x = barrier_part5.transform->position.x,
    .position.y = barrier_part5.transform->position.y + barrier_part5.collider->rect.height,
  });
}

void EntityFactory::createBarriers() {
  for (int i = 0; i < GameConfig::BARRIER_COUNT; i++) {
    Vec2 start_pos = GameConfig::BARRIER_START;
    start_pos.x += i * GameConfig::BARRIER_SPACING_X;
    createBarrier(start_pos);
  }
}

void EntityFactory::createGameWalls() {
  // FIX: spaghetti code nightmare fix later

  Entity &top_wall = game_state_.CreateEntity();
  top_wall.is_active = true;
  top_wall.bitmask.emplace(Bitmask{
    .layer = GameLayer::Wall,
    .mask = GameLayer::Player | GameLayer::Projectile, // I am adding projectiles
                                                       // to the top wall's mask
  });
  top_wall.wall_info.emplace(WallSide::Top);
  top_wall.transform.emplace(Transform{Vec2{window_width_ / 2, 0.0f}, Zero()});

  top_wall.collider.emplace(
    Collider{.shape = ColliderShape::Rectangle, .rect{(float)window_width_, GameConfig::WALL_THICKNESS}});

  // Bottom wall
  Entity &bottom_wall = game_state_.CreateEntity();
  bottom_wall.bitmask.emplace(Bitmask{
    .layer = GameLayer::Wall,
    .mask = GameLayer::Player | GameLayer::Projectile,
  });
  bottom_wall.wall_info.emplace(WallSide::Bottom);
  bottom_wall.is_active = true;
  bottom_wall.transform.emplace(Transform{
    Vec2{window_width_ / 2, (float)window_height_ - GameConfig::WALL_BOTTOM_OFFSET},
    Zero(),
  });
  bottom_wall.collider.emplace(Collider{
    .shape = ColliderShape::Rectangle,
    .rect{(float)window_width_, GameConfig::WALL_THICKNESS},
  });
  // Left wall
  Entity &left_wall = game_state_.CreateEntity();
  left_wall.bitmask.emplace(Bitmask{.layer = GameLayer::Wall, .mask = GameLayer::Player | GameLayer::Projectile});
  left_wall.wall_info.emplace(WallSide::Left);
  left_wall.is_active = true;
  left_wall.transform.emplace(Transform{
    Vec2{GameConfig::WALL_LR_OFFSET, window_height_ / 2},
    Zero(),
  });
  left_wall.collider.emplace(Collider{
    .shape = ColliderShape::Rectangle,
    .rect{GameConfig::WALL_THICKNESS, (float)window_height_},
  });
  // Right wall
  Entity &right_wall = game_state_.CreateEntity();
  right_wall.bitmask.emplace(Bitmask{
    .layer = GameLayer::Wall,
    .mask = GameLayer::Player,
  });
  right_wall.wall_info.emplace(WallSide::Right);
  right_wall.is_active = true;
  right_wall.transform.emplace(Transform{
    Vec2{(float)window_width_ - GameConfig::WALL_LR_OFFSET, window_height_ / 2.0f},
    Zero(),
  });
  right_wall.collider.emplace(Collider{
    .shape = ColliderShape::Rectangle,
    .rect{GameConfig::WALL_THICKNESS, (float)window_height_},
  });
}

void EntityFactory::createAlien(AlienType species, const Vec2 position) {
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
        return GameConfig::SCORE_SQUID;
      case AlienType::Crab:
        return GameConfig::SCORE_CRAB;
      case AlienType::Octopus:
        return GameConfig::SCORE_OCTOPUS;
      case AlienType::Ship:
        return GameConfig::SCORE_SHIP;
      default:
        return 0;
    }
  };

  // FIX: spaghetti code nightmare fix later

  Entity &alien = game_state_.CreateEntity();

  alien.bitmask.emplace(
    Bitmask{.layer = GameLayer::Enemy, .mask = GameLayer::Player | GameLayer::Wall | GameLayer::Projectile});

  alien.alien_info.emplace(Alien{.type = species, .score = species_to_score(species)});

  alien.velocity.emplace(GameConfig::ALIEN_SPEED);

  // WARN: experimental health component;
  alien.health.emplace(Health{.max_hp = GameConfig::ALIEN_MAX_HP});

  alien.transform.emplace(Transform{position, Down()});

  alien.sprite.emplace(Sprite{.frame_data = asset_manager.textures.at(species_to_key(species))});

  // Because it's cheap to hold just the explosion sprite in the entity, I'm
  // going to do that.
  alien.death_sprite.emplace(Sprite{.frame_data = asset_manager.textures.at("explosion")});

  const float sprite_width = alien.sprite->frame_data.frame1.w;
  const float sprite_height = alien.sprite->frame_data.frame1.h;

  alien.collider.emplace(Collider{.shape = ColliderShape::Rectangle, .rect{sprite_width, sprite_height}});

  alien.gun.emplace(Gun{.fire_flag = false});

  alien.is_active = true;
}

void EntityFactory::createAlienFormation() {
  // Authentic Space Invaders layout: 5 rows x 11 columns
  // Horizontal spacing: 16px (gives ~4-8px gap depending on sprite width)
  // Vertical spacing: 15px
  // Row 0: Squid (top)
  // Rows 1-2: Crab (middle)
  // Rows 3-4: Octopus (bottom)

  for (int row = 0; row < GameConfig::FORMATION_ROWS; row++) {
    for (int col = 0; col < GameConfig::FORMATION_COLS; col++) {
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

      const float x = GameConfig::FORMATION_START_X + col * GameConfig::FORMATION_STRIDE_X;
      const float y = GameConfig::FORMATION_START_Y + row * GameConfig::FORMATION_STRIDE_Y;

      createAlien(species, Vec2{x, y});
    }
  }
}

void EntityFactory::createMysteryShipSpawner() {
  Entity &spawner = game_state_.CreateEntity();
  spawner.mystery_ticker.emplace();
  spawner.bitmask.emplace();
}

// Constructor that uses an initalizer list because it's more efficient, it
// doesn't create temporary memory to copy over, because it's not code that runs
// in the constructor, it's the allocation of the struct itself.
EntityFactory::EntityFactory(GameState &game_state, AssetManager &asset_manager, float window_width,
                             float window_height)
    : game_state_(game_state), asset_manager(asset_manager), window_width_(window_width),
      window_height_(window_height) {}
