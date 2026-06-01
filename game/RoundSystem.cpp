
#include "RoundSystem.hpp"
#include "AssetManager.hpp"
#include "EntityFactory.hpp"
#include "EventSystem.hpp"
#include "GameState.hpp"
#include "RenderSystem.hpp"
#include "SDL3/SDL_render.h"
#include <cstdlib>
#include <ranges>

// FIX: Duplicate code need to remove testing refactor REMOVE REMOVE REMOVE
// Taken from EventSystem.cpp
bool is_formation_alien(Entity &entity) {
  if (!entity.alien_info.has_value()) {
    return false;
  }
  switch (entity.alien_info->type) {
  case AlienType::Squid:
    return true;
  case AlienType::Crab:
    return true;
  case AlienType::Octopus:
    return true;
  default:
    return false;
  }
}

bool have_aliens_breached(GameState &game_state) {
  auto has_breached = false;
  for (Entity &entity : game_state.entities) {
    if (!entity.alien_info.has_value()) {
      continue;
    }
    if (!entity.transform) {
      continue;
    }
    if (entity.transform->position.y >= 204.0f) {
      has_breached = true;
    }
  }
  return has_breached;
}

auto health_is_zero = [](Entity &entity) {
  return (entity.is_active && entity.health.has_value() &&
          entity.health->hp <= 0);
};

void update_alien_for_death(Entity &alien) {
  if (!alien.alien_info.has_value()) {
    SDL_Log(
        "Update alien for death passed entity without alien_info component");
    return;
  }
  alien.bitmask.reset();
  alien.velocity.reset();
  alien.health.reset();
  alien.collider.reset();
  if (alien.death_sprite.has_value()) {
    alien.sprite.emplace(alien.death_sprite.value());
    alien.alien_info->death_ticker.emplace(Ticker{.max_ticks = 45});
  }
}

void update_zero_health_entities(GameState &game_state) {
  for (auto &dead_entity :
       game_state.entities | std::views::filter(health_is_zero)) {

    // This feels jank but probably bullet proof (no pun intended)
    if (dead_entity.parent.has_value() && dead_entity.parent->was_player) {
      game_state.bullet_is_active = false;
      game_state.DestroyEntity(dead_entity.id);
    }

    if (!dead_entity.death_sprite.has_value()) {
      game_state.DestroyEntity(dead_entity.id);
    };

    if (dead_entity.alien_info.has_value()) {
      update_alien_for_death(dead_entity);
      game_state.score += dead_entity.alien_info->score;
      game_state.score_update = true;
    }

    if (is_formation_alien(dead_entity)) {
      game_state.number_of_aliens--;
    }
  };
}

void UpdateMysteryShipSpawner(GameState &game_state,
                              AssetManager &asset_manager) {

  // Find the spawner
  auto spawner = std::find_if(
      game_state.entities.begin(), game_state.entities.end(),
      [&](Entity &entity) { return entity.mystery_ticker.has_value(); });

  // if spawner doens't exist leave
  if (spawner == game_state.entities.end()) {
    SDL_Log("Spawner not found");
    return;
  }

  // if the spawner has waited 1536 ticks spawn ufo
  if (spawner->mystery_ticker->tick_count > 4400) {
    // spawn the ship entity to zero.
    spawner->mystery_ticker->tick_count = 0;

    auto old_ufo =
        std::find_if(game_state.entities.begin(), game_state.entities.end(),
                     [&](Entity &entity) {
                       // stops a bad access and then returns on whether or
                       // not it's a ship.
                       return entity.alien_info.has_value() &&
                              entity.alien_info->type == AlienType::Ship;
                     });
    // If old_ufo isn't found.
    if (old_ufo != game_state.entities.end()) {
      game_state.DestroyEntity(old_ufo->id);
    }

    // create the ufo
    auto new_ufo = &game_state.CreateEntity();

    new_ufo->is_active = true;

    new_ufo->alien_info.emplace(Alien{.type = AlienType::Ship, .score = 200});

    new_ufo->health.emplace(Health{.max_hp = 1});

    new_ufo->bitmask.emplace(
        Bitmask{.layer = GameLayer::Enemy, .mask = GameLayer::Projectile});

    new_ufo->sprite.emplace(
        Sprite{.frame_data = asset_manager.textures["spaceship"]});

    new_ufo->death_sprite.emplace(
        Sprite{.frame_data = asset_manager.textures["spaceship-death"]});

    auto sprite_width = new_ufo->sprite->frame_data.frame1.w;

    auto sprite_height = new_ufo->sprite->frame_data.frame1.h;

    new_ufo->collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                       .rect{sprite_width, sprite_height}});

    // decide which side it spawns on.
    auto side_multiplier = zero_or_one();

    // FIX: Magic number, window width, but I need to create a way to pass
    // that value.
    new_ufo->transform.emplace(
        Transform{.position = Vec2{.x = 256.0f * side_multiplier, .y = 25.0f}});

    auto direction = (side_multiplier == 0) ? Right() : Left();

    new_ufo->velocity.emplace(Velocity{.speed = direction * 0.75f});

    return;
  }

  spawner->mystery_ticker->tick_count++;
}

void update_game_lives(GameState &game_state) {

  auto player =
      std::find_if(game_state.entities.begin(), game_state.entities.end(),
                   [](Entity &entity) {
                     return ((entity.bitmask->layer & GameLayer::Player) ==
                             GameLayer::Player);
                   });

  game_state.number_of_lives = player->health->hp;
}

void RoundSystem::Update(GameState &game_state, RenderSystem &render_system,
                         EntityFactory &entity_factory,
                         AssetManager &asset_manager, SDL_Renderer *renderer,
                         bool &is_running) {

  if (new_round_flag_ == true) {

    // FIX: This is a magic operation, the rare evolution of a magic number!
    int i = 0;
    while (i < 100) {
      render_system.Update(game_state, renderer);
      i++;
    }

    entity_factory.createAlienFormation(new_round_offset_position_);

    new_round_flag_ = false;
  }

  if (game_state.number_of_aliens == 0) {
    new_round_flag_ = true;
    game_state.number_of_aliens = 55;
  }

  update_zero_health_entities(game_state);

  update_game_lives(game_state);

  if (have_aliens_breached(game_state)) {
    is_running = false;
  }
  UpdateMysteryShipSpawner(game_state, asset_manager);
}
