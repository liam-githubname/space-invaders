/* EventSystem.cpp
 * Authored by Liam Harvell
 */
// NOTE:=======================================================================
// 1. std::visit combined with std::variant gives you polymorphism without
// pointers, exhaustiveness checking, and zero overhead.
// 2. According to AI there is a common idiom for handling this. the Overload
// pattern
// 3. In C++, every lambda expression creates a unique, anonymous struct type
// with an operator() member
// 4. Operator overloading is a complicated subset of c++, look at
// "operators in c++" note in obsidian vault.
// Using a generic Lambda expression with the if constexpr
// 5. The "if constexpr" forces the compiler to only generate only the
// matching branch is compiled for each event type. The others will be
// discarded at compile time
// TODO:=======================================================================
// 3. I need to figure out if GameState.hpp should be included or not.
// FIXME:======================================================================
// ============================================================================

#include "EventSystem.hpp"
#include "Bitmask.hpp"
#include "Events.hpp"
#include "GameState.hpp"
#include "Util.hpp"
// TODO: Remove after the logging isn't required
#include <SDL3/SDL_log.h>
#include <cstdlib>
#include <ranges>

void update_alien_for_death(Entity &alien) {
  if (!alien.alien_info.has_value()) {
    SDL_Log(
        "Update alien for death passed entity without alien_info component");
    return;
  }
  alien.bitmask.reset();
  alien.velocity.reset();
  alien.collider.reset();
  if (alien.death_sprite.has_value()) {
    alien.sprite.emplace(alien.death_sprite.value());
    alien.alien_info->death_ticker.emplace(Ticker{.max_ticks = 45});
  }
}

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

class EventSystem;
// This is the main use of this class here.
void EventSystem::HandleCollisionPayload(const CollisionPayload &payload,
                                         GameState &game_state) {
  // Entity pointer holds the address of an entity
  Entity *entity_a = nullptr;
  Entity *entity_b = nullptr;
  // Handle Wall Collisions
  // ==================================================== Searches through the
  // entities to find the ones from the payload.
  for (auto &entity : game_state.entities) {
    if (entity.id == payload.entity_a_id) {
      // The address of the entity reference or the address of the entity
      entity_a = &entity;
    }
    if (entity.id == payload.entity_b_id) {
      entity_b = &entity;
    }
  } // WARN: There is a c++ function called find_if that I could use to have
  // The dereferenced entity pointer should hold a entity reference
  WallCollisionHandler(*entity_a, *entity_b, game_state);
  PlayerBulletCollisionHandler(*entity_a, *entity_b, game_state);

  // ==========================================================================
}

void EventSystem::PlayerBulletCollisionHandler(Entity &entity_a,
                                               Entity &entity_b,
                                               GameState &game_state) {
  // Shouldn't theoretically be possible but useful nonetheless

  if ((entity_a.bitmask->layer & entity_b.bitmask->mask) == GameLayer::None) {
    return;
  }
  if ((entity_b.bitmask->layer & entity_a.bitmask->mask) == GameLayer::None) {
    return;
  }
  // If it's not a bullet then leave.
  if ((entity_a.bitmask->layer != GameLayer::Projectile) &&
      (entity_b.bitmask->layer != GameLayer::Projectile)) {
    return;
  }

  // Destroy the bullet.
  if (entity_a.bitmask->layer == GameLayer::Projectile) {
    game_state.DestroyEntity(entity_a.id);
    game_state.bullet_is_active = false;
  } else {
    game_state.DestroyEntity(entity_b.id);
    game_state.bullet_is_active = false;
  }

  // If the other entity is an enemy destroy the enemy.
  if (entity_a.bitmask->layer == GameLayer::Enemy &&
      entity_a.alien_info.has_value()) {

    update_alien_for_death(entity_a);

    game_state.score += entity_a.alien_info->score;
    game_state.score_update = true;

    if (is_formation_alien(entity_a)) {
      game_state.number_of_aliens--;
    }

    // FIX: Look into making this only a check of alien_info.
    // Otherwise replace it with a function.
  } else if (entity_b.bitmask->layer == GameLayer::Enemy &&
             entity_b.alien_info.has_value()) {

    update_alien_for_death(entity_b);

    game_state.score += entity_b.alien_info->score;
    game_state.score_update = true;

    if (is_formation_alien(entity_b)) {
      game_state.number_of_aliens--;
    }
  }

  return;
}

// NOTE: I know it's not pretty but I'm running out of time.
void EventSystem::WallCollisionHandler(Entity &entity_a, Entity &entity_b,
                                       GameState &game_state) {
  // Check to see if it's a wall and something with velocity.
  bool a_velocity = entity_a.velocity.has_value();
  bool b_velocity = entity_b.velocity.has_value();
  bool a_wall = entity_a.wall_info.has_value();
  bool b_wall = entity_b.wall_info.has_value();
  if (!(a_velocity && b_wall) && !(b_velocity && a_wall))
    return;

  // This is the easiest way to assign the player.
  auto &entity = (a_velocity) ? entity_a : entity_b;
  // This is redundant but a more elegant solution isn't coming to mind rn
  auto &wall = (a_wall) ? entity_a : entity_b;

  float wall_x, wall_y;
  WallSide which_wallside;

  wall_x = wall.transform->position.x;
  wall_y = wall.transform->position.y;
  which_wallside = wall.wall_info->side;
  Vec2 transform_update;

  auto wall_transform_update = [&](WallSide wallside) {
    // I wasnt' addign the walls width? I think that's causing it to get stuck?
    switch (wallside) {
    case WallSide::Top:
      return Vec2{
          .x = entity.transform->position.x,
          .y = wall_y + entity.collider->rect.height / 2,
      };
      break;
    case WallSide::Bottom:
      return Vec2{
          .x = entity.transform->position.x,
          .y = wall_y - entity.collider->rect.height / 2,
      };
      break;
    case WallSide::Left:
      return Vec2{
          .x = wall_x + entity.collider->rect.width / 2 +
               wall.collider->rect.width / 2,
          .y = entity.transform->position.y,
      };
      break;
    case WallSide::Right:
      return Vec2{
          .x = wall_x - entity.collider->rect.width / 2 -
               wall.collider->rect.width / 2,
          .y = entity.transform->position.y,
      };
      break;
    default:
      SDL_Log("in default switch case in EventSystem.cpp");
      break;
    }
  };

  // If an alien hits the wall add the altermovement component
  // So I went looking to see if C++ had the ability to filter a vector with a
  // lambda and then to iterate over it and it does!
  // Create a Predicate (lambda expression)
  // auto is_formation_alien = [](Entity &entity) {
  //   if (!entity.alien_info.has_value()) {
  //     return false;
  //   }
  //   switch (entity.alien_info->type) {
  //   case AlienType::Squid:
  //     return true;
  //     break;
  //   case AlienType::Crab:
  //     return true;
  //     break;
  //   case AlienType::Octopus:
  //     return true;
  //     break;
  //   default:
  //     return false;
  //   }
  // };

  if (is_formation_alien(entity)) {

    if (wall.wall_info->side == WallSide::Top ||
        wall.wall_info->side == WallSide::Bottom) {
      game_state.DestroyEntity(entity.id);
    }

    auto new_direction_sign =
        (wall.wall_info->side == WallSide::Right) ? -1.0f : 1.0f;

    auto new_movement_intent = AlterMovement{
        .position_update = Vec2{.x = 0.0f, .y = 2.0f},
        .speed_assignment = Vec2{.x = new_direction_sign, .y = 0.0f},
        .suppress_velocity = false,
    };

    // Then you create a std::view by using your predicate as the filter
    for (auto &enemy :
         game_state.entities | std::views::filter(is_formation_alien)) {

      // Then you apply whatever you want to the members of the view.
      enemy.movement_mod.emplace(new_movement_intent);
    }

  } else {

    auto player_movement_intent = AlterMovement{
        .speed_assignment = Vec2{.x = 1.0f, .y = 0.0f},
        .suppress_velocity = true,
    };

    entity.movement_mod.emplace(player_movement_intent);
    entity.transform->position = wall_transform_update(which_wallside);
  }
}

void EventSystem::HandleScorePayload(const ScorePayload &payload,
                                     GameState &game_state) {
  game_state.score += payload.points;
}

void EventSystem::ProcessEvents(GameState &game_state) {
  for (auto &event : game_state.event_queue.GetEvents()) {
    std::visit(
        Overload{
            [&](const CollisionPayload payload) {
              HandleCollisionPayload(payload, game_state);
            },
            [&](const DeathPayload payload) {
              SDL_Log("Consumed DeathPayload");
            },
            // TODO: I'm probably not going to use this, I'll have to remove it.
            [&](const ScorePayload payload) {
              HandleScorePayload(payload, game_state);
            },
            [&](const HitPayload payload) { /*SDL_Log("HitPayload event");*/ },
            //====================Add new payloads here===================
        },
        event);
  }
  game_state.event_queue.ClearEventQueue();
}
