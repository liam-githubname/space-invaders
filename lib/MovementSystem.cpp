// NOTE:=======================================================================
// 1. The movement system updates the transform based on the velocity.
// 2. The player entity uses the PlayerInput component.
// TODO:=======================================================================
// 1. Add multiderectional movement.
// 2. Normalize diagonal movement?
// ============================================================================
#include "MovementSystem.hpp"
#include "GameConfig.hpp"
#include "GameState.hpp"
#include <SDL3/SDL_log.h>

class MovementSystem;

void MovementSystem::Update(GameState &game_state, float current_frame_time) {
  // NOTE: The real Space Invaders got faster with every alien killed, because
  // it freed up cpu cycles. There is a way to do this here. I could reduce the
  // time threshold to move the aliens with each ones death. I could increase
  // their velocity (I think that would look least like the original).
  // But the movement already doens't look exactly like the arcade as only one
  // alien was updated per frame back then. If I were to achieve that today I
  // would have to write some "sophisticated" algorithm to move them one at a
  // time and keep up with each other.
  // It also explains how the original space invaders got around this multiple
  // wall collision problem that I had. Only one alien could touch the wall at
  // any given moment.

  auto number_of_aliens_alive = game_state.total_number_of_aliens - game_state.number_of_aliens;
  auto alien_speed_multiplier = number_of_aliens_alive;

  auto has_been_alien_step_time =
    ((current_frame_time - last_time) >
     GameConfig::ALIEN_STEP_BASE_NS - alien_speed_multiplier * GameConfig::ALIEN_STEP_PER_KILL_NS);

  for (auto &entity : game_state.entities) {
    // Guard against dead entities.
    if (!entity.is_active) {
      continue;
    }
    // Update the transform component with the velocity component
    if (!entity.velocity.has_value() || !entity.transform.has_value()) {
      continue;
    }

    // If it hasnt been long enough and you're looking an alien stop looking.
    // The spaceship now bypasses the time blocker
    if (!has_been_alien_step_time && entity.alien_info.has_value() && entity.alien_info->type != AlienType::Ship) {
      continue;
    }

    bool suppress = false;

    // The system sees an intent component
    if (entity.movement_mod.has_value()) {
      // if the intent component has updates for position, apply those directly
      if (entity.movement_mod->position_update.has_value()) {
        entity.transform->position += entity.movement_mod->position_update.value();
      }

      // if the new speed_assignment is there assign entities speed
      entity.velocity->speed = entity.movement_mod->speed_assignment.has_value()
                                 ? entity.movement_mod->speed_assignment.value()
                                 : entity.velocity->speed;

      suppress = entity.movement_mod->suppress_velocity;

      // clear the intent component
      entity.movement_mod = {};
    }

    auto new_entity_position = One();

    if (entity.player_input) {
      new_entity_position *= entity.player_input->move;
    }
    new_entity_position *= entity.velocity->speed;

    // WARN: This is so the renederer knows when to swap to the other sprite,
    // if it exists.
    if (entity.sprite->frame_data.frame2.has_value()) {
      entity.sprite->step_1 = (entity.sprite->step_1) ? false : true;
    }

    if (suppress) {
      continue;
    }

    entity.transform->position += new_entity_position;

    last_time = has_been_alien_step_time ? current_frame_time : last_time;
  }
}
