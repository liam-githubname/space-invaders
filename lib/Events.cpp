#include "Events.hpp"
#include "AlienComponents.hpp"
#include "GameState.hpp"
#include <cstdint>
#include <ranges>

CollisionEvent::CollisionEvent(uint32_t entity_a_id, uint32_t entity_b_id)
    : entity_a_id(entity_a_id), entity_b_id(entity_b_id) {};

void CollisionEvent::execute(GameState &game_state) const {
  // Entity pointer holds the address of an entity
  Entity *entity_a = nullptr;
  Entity *entity_b = nullptr;
  // Handle Wall Collisions
  // ==================================================== Searches through the
  // entities to find the ones from the payload.
  for (Entity &entity : game_state.entities) {
    if (entity.id == entity_a_id) {
      // The address of the entity reference or the address of the entity
      entity_a = &entity;
    }
    if (entity.id == entity_b_id) {
      entity_b = &entity;
    }
  }
  WallCollisionHandler(*entity_a, *entity_b, game_state);
  BulletCollisionHandler(*entity_a, *entity_b);

  // ==========================================================================
}

void CollisionEvent::BulletCollisionHandler(Entity &entity_a, Entity &entity_b) const {
  // If it's not a bullet then leave.
  if ((entity_a.bitmask->layer != GameLayer::Projectile) && (entity_b.bitmask->layer != GameLayer::Projectile)) {
    return;
  }

  if (entity_a.health.has_value()) {
    --entity_a.health->hp;
  }

  if (entity_b.health.has_value()) {
    --entity_b.health->hp;
  }
}

void CollisionEvent::WallCollisionHandler(Entity &entity_a, Entity &entity_b, GameState &game_state) const {
  // Check to see if it's a wall and something with velocity.
  const bool a_velocity = entity_a.velocity.has_value();
  const bool b_velocity = entity_b.velocity.has_value();
  const bool a_wall = entity_a.wall_info.has_value();
  const bool b_wall = entity_b.wall_info.has_value();
  if (!(a_velocity && b_wall) && !(b_velocity && a_wall))
    return;

  // This is the easiest way to assign the
  Entity &entity = (a_velocity) ? entity_a : entity_b;
  // This is redundant but a more elegant solution isn't coming to mind rn
  Entity &wall = (a_wall) ? entity_a : entity_b;

  const float wall_x = wall.transform->position.x;
  const float wall_y = wall.transform->position.y;
  const WallSide which_wallside = wall.wall_info->side;

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
          .x = wall_x + entity.collider->rect.width / 2 + wall.collider->rect.width / 2,
          .y = entity.transform->position.y,
        };
        break;
      case WallSide::Right:
        return Vec2{
          .x = wall_x - entity.collider->rect.width / 2 - wall.collider->rect.width / 2,
          .y = entity.transform->position.y,
        };
        break;
      default:
        SDL_Log("in default switch case in EventSystem.cpp");
        break;
    }
  };

  std::optional<Alien> &alien_information = entity.alien_info;
  if (alien_information.has_value() && alien_information->is_formation_alien()) {
    const float new_direction_sign = (wall.wall_info->side == WallSide::Right) ? -1.0f : 1.0f;

    const AlterMovement new_movement_intent = AlterMovement{
      // FIX: Magic number alien jump down
      .position_update = Vec2{.x = 0.0f, .y = 2.0f},
      .speed_assignment = Vec2{.x = new_direction_sign, .y = 0.0f},
      .suppress_velocity = false,
    };

    auto formation_filter = [](Entity &entity) {
      return entity.alien_info.has_value() ? entity.alien_info->is_formation_alien() : false;
    };

    for (Entity &enemy : game_state.entities | std::views::filter(formation_filter)) {
      enemy.movement_mod.emplace(new_movement_intent);
    }

  } else {
    const AlterMovement player_movement_intent = AlterMovement{
      .speed_assignment = Vec2{.x = 1.0f, .y = 0.0f},
      .suppress_velocity = true,
    };
    entity.movement_mod.emplace(player_movement_intent);
    entity.transform->position = wall_transform_update(which_wallside);
  }
}
