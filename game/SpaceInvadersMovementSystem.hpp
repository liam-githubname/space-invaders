#pragma once

#include "GameState.hpp"

class SpaceInvadersMovementSystem {
public:
  void Update(GameState &game_state);

private:
  float alien_movement_speed_x;
  float alien_movement_speed_y;

  void StopVerticalMovement(Entity *entity);
};
