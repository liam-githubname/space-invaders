// NOTE:=======================================================================
// 1. The movement system updates the transform based on the velocity.
// 2. The player entity uses the PlayerInput component.
// TODO:=======================================================================
// ============================================================================
#include "MovementSystem.hpp"
#include "CoreComponents.hpp"
#include "GameConfig.hpp"
#include "GameState.hpp"
#include <SDL3/SDL_log.h>
#include <optional>

class MovementSystem;

void MovementSystem::Update(GameState &game_state, float current_frame_time) {
  /* NOTE:
   * The real Space Invaders got faster with every alien killed, because
   * it freed up cpu cycles. Every frame the cabinet moved one alien, so as the aliens we're killed and they didn't need
   * to be updated and the cpu could cycle through the remaining aliens faster. I am obviously not limited by the CPUs
   * they had in 1978, so we're going to hack together this kind of behavior somehow else. The solution I came up with
   * is measure the amount of time passed and move the aliens each time that threshold is met.
   */

  const int number_of_aliens_killed = game_state.total_number_of_aliens - game_state.number_of_aliens;
  const int alien_speed_multiplier = number_of_aliens_killed;
  const float time_since_last_frame = current_frame_time - last_time;
  const float alien_step_time_threshold =
    GameConfig::ALIEN_STEP_BASE_NS - alien_speed_multiplier * GameConfig::ALIEN_STEP_PER_KILL_NS;
  const bool has_been_alien_step_time = time_since_last_frame > alien_step_time_threshold;
  last_time = has_been_alien_step_time ? current_frame_time : last_time;

  for (Entity &entity : game_state.entities) {
    if (!entity.is_active) {
      continue;
    }
    if (!entity.velocity.has_value() || !entity.transform.has_value()) {
      continue;
    }
    // create locals to avoid multiple dereferences.
    Velocity &entity_velocity = entity.velocity.value();
    Transform &entity_transform = entity.transform.value();

    if (!has_been_alien_step_time && entity.alien_info.has_value() && entity.alien_info->type != AlienType::Ship) {
      continue;
    }

    bool suppress = false;
    // The system sees an intent component

    if (entity.movement_mod.has_value()) {
      // if the intent component has updates for position, apply those directly
      const std::optional<Vec2> &new_position_vector = entity.movement_mod->position_update;
      if (new_position_vector.has_value()) {
        entity_transform.position += new_position_vector.value();
      }

      Vec2 &entity_speed = entity_velocity.speed;
      std::optional<Vec2> &new_speed = entity.movement_mod->speed_assignment;

      entity_velocity.speed = new_speed.has_value() ? new_speed.value() : entity_velocity.speed;

      suppress = entity.movement_mod->suppress_velocity;

      // clear the intent component
      entity.movement_mod = std::nullopt;
    }

    // calculate the new position of the entity
    Vec2 new_entity_position = One();

    if (entity.player_input.has_value()) {
      new_entity_position *= entity.player_input->move;
    }
    new_entity_position *= entity_velocity.speed;

    const bool entity_has_animation = entity.sprite->frame_data.frame2.has_value();
    bool &is_sprite_on_step_one = entity.sprite->step_1;

    if (entity_has_animation) {
      is_sprite_on_step_one = !is_sprite_on_step_one;
    }

    if (suppress) {
      continue;
    }

    entity_transform.position += new_entity_position;
  }
}
