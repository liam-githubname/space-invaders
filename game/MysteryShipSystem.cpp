#include "MysteryShipSystem.hpp"
#include "AssetManager.hpp"
#include "Bitmask.hpp"
#include "GameState.hpp"
#include "Util.hpp"
#include <cstdlib>
#include <random>

int zero_or_one() {

  std::random_device rd;
  // Mersenne Twister engine
  std::mt19937 generator(rd());
  // Give me an integer evenly distributed between 1 and 100
  std::uniform_int_distribution<int> intDist(0, 1);

  // pass the generator into the distribution
  return intDist(generator);
}

void MysteryShipSystem::Update(GameState &game_state,
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
                       // stops a bad access and then returns on whether or not
                       // it's a ship.
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

    // FIX: Magic number, window width, but I need to create a way to pass that
    // value.
    new_ufo->transform.emplace(
        Transform{.position = Vec2{.x = 256.0f * side_multiplier, .y = 25.0f}});

    auto direction = (side_multiplier == 0) ? Right() : Left();

    new_ufo->velocity.emplace(Velocity{.speed = direction * 0.75f});

    return;
  }

  spawner->mystery_ticker->tick_count++;
}
