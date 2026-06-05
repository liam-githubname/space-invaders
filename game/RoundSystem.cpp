
#include "RoundSystem.hpp"
#include "AssetManager.hpp"
#include "EntityFactory.hpp"
#include "GameConfig.hpp"
#include "GameState.hpp"
#include "RenderSystem.hpp"
#include "SDL3/SDL_render.h"
#include <cstdlib>
#include <ranges>

bool have_aliens_breached(const GameState &game_state) {
  bool has_breached = false;
  for (const Entity &entity : game_state.entities) {
    if (!entity.alien_info.has_value()) {
      continue;
    }
    if (!entity.transform) {
      continue;
    }
    if (entity.transform->position.y >= GameConfig::ALIEN_BREACH_Y) {
      has_breached = true;
    }
  }
  return has_breached;
}

auto health_is_zero = [](const Entity &entity) {
  return (entity.is_active && entity.health.has_value() && entity.health->hp <= 0);
};

void update_alien_for_death(Entity &alien) {
  if (!alien.alien_info.has_value()) {
    SDL_Log("Update alien for death passed entity without alien_info component");
    return;
  }
  alien.bitmask.reset();
  alien.velocity.reset();
  alien.health.reset();
  alien.collider.reset();
  if (alien.death_sprite.has_value()) {
    alien.sprite.emplace(alien.death_sprite.value());
    alien.alien_info->death_ticker.emplace(Ticker{.max_ticks = GameConfig::ALIEN_DEATH_TICKS});
  }
}

void update_zero_health_entities(GameState &game_state) {
  for (Entity &dead_entity : game_state.entities | std::views::filter(health_is_zero)) {
    // This feels jank but probably bullet proof (no pun intended)
    if (dead_entity.parent.has_value() && dead_entity.parent->was_player) {
      game_state.bullet_is_active = false;
      game_state.DestroyEntity(dead_entity.id);
    }

    if (!dead_entity.death_sprite.has_value()) {
      game_state.DestroyEntity(dead_entity.id);
    };

    std::optional<Alien> &alien_information = dead_entity.alien_info;
    if (alien_information.has_value()) {
      update_alien_for_death(dead_entity);
      game_state.score += dead_entity.alien_info->score;
      game_state.score_update = true;
    }

    if (alien_information.has_value() && alien_information->is_formation_alien()) {
      game_state.number_of_aliens--;
    }
  };
}

void UpdateMysteryShipSpawner(GameState &game_state, const AssetManager &asset_manager) {
  // Find the spawner
  auto spawner = std::find_if(game_state.entities.begin(), game_state.entities.end(),
                              [&](Entity &entity) { return entity.mystery_ticker.has_value(); });

  // if spawner doens't exist leave
  if (spawner == game_state.entities.end()) {
    SDL_Log("Spawner not found");
    return;
  }

  if (spawner->mystery_ticker->tick_count > GameConfig::MYSTERY_SHIP_SPAWN_TICKS) {
    // spawn the ship entity to zero.
    spawner->mystery_ticker->tick_count = 0;

    auto old_ufo = std::find_if(game_state.entities.begin(), game_state.entities.end(), [&](Entity &entity) {
      // stops a bad access and then returns on whether or
      // not it's a ship.
      return entity.alien_info.has_value() && entity.alien_info->type == AlienType::Ship;
    });
    // If old_ufo isn't found.
    if (old_ufo != game_state.entities.end()) {
      game_state.DestroyEntity(old_ufo->id);
    }

    // create the ufo
    Entity *new_ufo = &game_state.CreateEntity();

    new_ufo->is_active = true;

    new_ufo->alien_info.emplace(Alien{.type = AlienType::Ship, .score = GameConfig::MYSTERY_SHIP_SCORE});

    new_ufo->health.emplace(Health{.max_hp = GameConfig::MYSTERY_SHIP_MAX_HP});

    new_ufo->bitmask.emplace(Bitmask{.layer = GameLayer::Enemy, .mask = GameLayer::Projectile});

    new_ufo->sprite.emplace(Sprite{.frame_data = asset_manager.textures.at("spaceship")});

    new_ufo->death_sprite.emplace(Sprite{.frame_data = asset_manager.textures.at("spaceship-death")});

    const float sprite_width = new_ufo->sprite->frame_data.frame1.w;

    const float sprite_height = new_ufo->sprite->frame_data.frame1.h;

    new_ufo->collider.emplace(Collider{.shape = ColliderShape::Rectangle, .rect{sprite_width, sprite_height}});

    // decide which side it spawns on.
    const int side_multiplier = zero_or_one();

    new_ufo->transform.emplace(Transform{
      .position = Vec2{.x = (float)GameConfig::WINDOW_WIDTH * side_multiplier, .y = GameConfig::MYSTERY_SHIP_Y}});

    const Vec2 direction = (side_multiplier == 0) ? Right() : Left();

    new_ufo->velocity.emplace(Velocity{.speed = direction * GameConfig::MYSTERY_SHIP_SPEED});

    return;
  }

  spawner->mystery_ticker->tick_count++;
}

void update_game_lives(GameState &game_state, int hp_bonus) {
  auto player = std::find_if(game_state.entities.begin(), game_state.entities.end(), [](Entity &entity) {
    return ((entity.bitmask->layer & GameLayer::Player) == GameLayer::Player);
  });

  player->health->hp += hp_bonus;

  if (game_state.number_of_lives != player->health->hp) {
    game_state.number_of_lives = player->health->hp;
    game_state.lives_update = true;
  }
}

void is_player_alive(const GameState &game_state, bool &is_running) {
  if (game_state.number_of_lives <= 0) {
    is_running = false;
  }
}

void RoundSystem::Update(GameState &game_state, RenderSystem &render_system, EntityFactory &entity_factory,
                         const AssetManager &asset_manager, SDL_Renderer *renderer, bool &is_running) {
  // Begin new Round code
  if (new_round_flag_ == true) {
    int i = 0;
    while (i < GameConfig::ROUND_TRANSITION_RENDERS) {
      render_system.Update(game_state, renderer);
      i++;
    }

    entity_factory.createAlienFormation();

    update_game_lives(game_state, GameConfig::LIVES_BONUS_PER_ROUND);

    new_round_flag_ = false;
  }

  // set the flag to start a new round next frame
  if (game_state.number_of_aliens == 0) {
    new_round_flag_ = true;
    game_state.number_of_aliens = GameConfig::ALIENS_PER_ROUND;
  }

  // This is what needs to run every frame
  update_zero_health_entities(game_state);
  update_game_lives(game_state, 0);
  is_player_alive(game_state, is_running);

  if (have_aliens_breached(game_state)) {
    is_running = false;
  }
  UpdateMysteryShipSpawner(game_state, asset_manager);
}
