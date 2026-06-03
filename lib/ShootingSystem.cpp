/* ShootingSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. Raycast.hpp has the definition of a raycast as well as the equation.
// 2. The raycast functions are not normalized right now. That needs to be fixed
// 3. Very important the Raycast functions return the distance if a hit occurred
// otherwise it returns a nullopt. The shooting system will check if that
// distance is too far. This removes game knoweledge burden from Raycast.hpp.
// TODO:=======================================================================
// FIX:========================================================================
// ============================================================================
#include "ShootingSystem.hpp"
#include "AssetManager.hpp"
#include "Bitmask.hpp"
#include "Events.hpp"
#include "GameConfig.hpp"
#include "GameState.hpp"
#include "Raycast.hpp"
#include "Util.hpp"
#include <cstdint>
#include <optional>

void update_player_shooting(GameState &game_state, AssetManager &asset_manager, Entity &player) {
  if (game_state.bullet_is_active) {
    return;
  }
  // Player shooting logic
  if (player.bitmask->layer == GameLayer::Player && player.player_input->is_firing) {
    auto &player_bullet = game_state.CreateEntity();
    player_bullet.is_active = true;
    player_bullet.parent.emplace(ParentEntityClass{.was_player = true});

    player_bullet.sprite.emplace(Sprite{.frame_data = asset_manager.textures["bullet"]});
    auto col_width = player_bullet.sprite->frame_data.frame1.w;
    auto col_height = player_bullet.sprite->frame_data.frame1.h;

    player_bullet.health.emplace(Health{.max_hp = GameConfig::BULLET_MAX_HP});
    player_bullet.collider.emplace(Collider{.shape = ColliderShape::Rectangle, .rect{col_width, col_height}});
    player_bullet.velocity.emplace(Velocity{.speed = GameConfig::PLAYER_BULLET_SPEED});
    player_bullet.bitmask.emplace(Bitmask{
      .layer = GameLayer::Projectile,
      .mask = GameLayer::Wall | GameLayer::Enemy,
    });
    player_bullet.transform.emplace(Transform{.position = player.transform->position, .direction = Up()});
    player_bullet.transform->position.y += GameConfig::PLAYER_BULLET_OFFSET_Y;

    game_state.bullet_is_active = true;
  }
}

bool ray_cast_hits_alien(GameState &game_state, Entity &entity) {
  uint32_t hit_entity_id = -1;
  // The raycasting portion of the shooting system
  if (!entity.gun.has_value() || !entity.gun->fire_flag)
    //
    return true;

  auto is_shortest_hit_alien = false;
  float shortest_distance = INFINITY;

  // Raycast
  Raycast::Ray ray{.origin = Vec2{entity.transform->position.x, entity.transform->position.y},
                   .direction = Vec2{entity.transform->direction.x, entity.transform->direction.y}};

  for (auto &other_entity : game_state.entities) {
    // Skip if it's looking at itself
    if (entity.id == other_entity.id) {
      continue;
    }

    // // skip if the caster isn't concerned with other entity's layer.
    // if ((entity.bitmask->mask & other_entity.bitmask->layer) ==
    //     GameLayer::None) {
    //   continue;
    // }

    if (other_entity.collider->shape == ColliderShape::Rectangle) {
      float min_x = other_entity.transform->position.x - other_entity.collider->rect.width / 2;
      float max_x = other_entity.transform->position.x + other_entity.collider->rect.width / 2;
      float min_y = other_entity.transform->position.y - other_entity.collider->rect.height / 2;
      float max_y = other_entity.transform->position.y + other_entity.collider->rect.height / 2;

      // NOTE: This is the first time I'm returning an optional like this.
      std::optional<float> distance = Raycast::RayAgainstAABB(ray, min_x, min_y, max_x, max_y);

      // I just learned that you don't need .has_value()
      // the optional has a built-in operator bool().
      // Which is a cool feature, but damn 2026-04-27.
      //
      if (!distance.has_value()) {
        continue;
      }
      if (*distance >= entity.gun->distance) {
        // SDL_Log("Distance value %f", *distance);
        continue;
      }

      // After further research, I could remove the .value() here as well.
      // With the addition of optionals pointer semantics was kept in
      // mind. so *distance is the same as distance.value();
      if (*distance < shortest_distance) {
        shortest_distance = distance.value();
        // hit_entity_id = other_entity.id;
        // SDL_Log("shortest distance %f", shortest_distance);
        is_shortest_hit_alien = (other_entity.alien_info.has_value()) ? true : false;
      }
    }
    // TODO: #3 Check Circle colliders.
  }
  return is_shortest_hit_alien;
}

uint32_t raycast(GameState &game_state, Entity &entity) {
  uint32_t hit_entity_id = -1;
  // The raycasting portion of the shooting system
  if (!entity.gun.has_value() || !entity.gun->fire_flag)
    return true;

  float shortest_distance = INFINITY;

  // Raycast
  Raycast::Ray ray{.origin = Vec2{entity.transform->position.x, entity.transform->position.y},
                   .direction = Vec2{entity.transform->direction.x, entity.transform->direction.y}};

  for (auto &other_entity : game_state.entities) {
    // Skip if it's looking at itself
    if (entity.id == other_entity.id) {
      continue;
    }
    // skip if the caster isn't concerned with other entity's layer.
    if ((entity.bitmask->mask & other_entity.bitmask->layer) == GameLayer::None) {
      continue;
    }

    if (other_entity.collider->shape == ColliderShape::Rectangle) {
      float min_x = other_entity.transform->position.x - other_entity.collider->rect.width / 2;
      float max_x = other_entity.transform->position.x + other_entity.collider->rect.width / 2;
      float min_y = other_entity.transform->position.y - other_entity.collider->rect.height / 2;
      float max_y = other_entity.transform->position.y + other_entity.collider->rect.height / 2;

      // NOTE: This is the first time I'm returning an optional like this.
      std::optional<float> distance = Raycast::RayAgainstAABB(ray, min_x, min_y, max_x, max_y);

      // I just learned that you don't need .has_value()
      // the optional has a built-in operator bool().
      // Which is a cool feature, but damn 2026-04-27.
      //
      if (!distance.has_value()) {
        continue;
      }
      if (*distance >= entity.gun->distance) {
        // SDL_Log("Distance value %f", *distance);
        continue;
      }

      // After further research, I could remove the .value() here as well.
      // With the addition of optionals pointer semantics was kept in
      // mind. so *distance is the same as distance.value();
      if (*distance < shortest_distance) {
        shortest_distance = distance.value();
        hit_entity_id = other_entity.id;
        // SDL_Log("shortest distance %f", shortest_distance);
      }
    }
    // TODO: #3 Check Circle colliders.
  }

  // If the raycast fits the component distance then emit a event otherwise just
  // return the id of the other entity.
  if (shortest_distance < entity.gun->distance) {
    //  WARN: These might be moving away from SRP, I'll have to rethink this
    game_state.event_queue.PushEvent(HitPayload{
      .entity_a_id = entity.id,
      .entity_b_id = hit_entity_id,
    });
  }
  return hit_entity_id;
}

void update_alien_shooting(GameState &game_state, Entity &entity, AssetManager &asset_manager) {
  if (ray_cast_hits_alien(game_state, entity)) {
    return;
  }

  auto &enemy_bullet = game_state.CreateEntity();
  enemy_bullet.is_active = true;

  enemy_bullet.health.emplace(Health{.max_hp = GameConfig::BULLET_MAX_HP});
  enemy_bullet.sprite.emplace(Sprite{.frame_data = asset_manager.textures["bullet"]});
  auto col_width = enemy_bullet.sprite->frame_data.frame1.w;
  auto col_height = enemy_bullet.sprite->frame_data.frame1.h;

  enemy_bullet.collider.emplace(Collider{.shape = ColliderShape::Rectangle, .rect{col_width, col_height}});
  enemy_bullet.velocity.emplace(Velocity{.speed = GameConfig::ENEMY_BULLET_SPEED});
  enemy_bullet.bitmask.emplace(Bitmask{
    .layer = GameLayer::Projectile,
    .mask = GameLayer::Player | GameLayer::Wall,
  });
  enemy_bullet.transform.emplace(Transform{.position = entity.transform->position, .direction = Down()});
  enemy_bullet.transform->position.y += entity.sprite->frame_data.frame1.h / 2;

  // Adding a death timer for the entity
  // enemy_bullet.time_death.emplace(TimedDeath{.ticker{.max_ticks = 1000}});
}

void ShootingSystem::Update(GameState &game_state, AssetManager &asset_manager) {
  for (auto &entity : game_state.entities) {
    // early guards
    if (!entity.is_active)
      continue;
    if (!entity.transform)
      continue;

    // FIX: I feel like it doens't make much sense being here
    if (entity.time_death.has_value()) {
      entity.time_death->ticker.tick_count++;
      if (entity.time_death->ticker.tick_count > entity.time_death->ticker.max_ticks) {
        game_state.DestroyEntity(entity.id);
      }
    }

    if (entity.player_input.has_value()) {
      update_player_shooting(game_state, asset_manager, entity);
    }

    // raycast downward to check if aliens have a clear line of sight
    // if they do have, fire at random.
    if (entity.alien_info.has_value()) {
      update_alien_shooting(game_state, entity, asset_manager);
    }
  }
}
// 2. Check non player shooting.
