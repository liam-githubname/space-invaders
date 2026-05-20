/* MovementSystem.hpp
 * Authored by Liam Harvell
 */
#pragma once

#include "GameState.hpp"

class GameState;

class MovementSystem {
public:
  void Update(GameState &game_state, float current_frame_time);

private:
  float acceleration = 10.0f;
  float last_time = 0.0f;
};
