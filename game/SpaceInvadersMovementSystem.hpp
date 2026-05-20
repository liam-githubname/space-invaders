#pragma once

#include "GameState.hpp"

class SpaceInvadersMovementSystem {
public:
  void Update(GameState &game_state, float new_time, float dt);

private:
  float timer = 0.0;
  float alien_movement_speed_x = 60.0;
  float alien_movement_speed_y = 5.0;
};
