/* MovementSystem.hpp
 * Authored by Liam Harvell
 */
#pragma once

#include "GameState.hpp"

class GameState;

class MovementSystem {
public:
  void Update(GameState &game_state, float delta_time);

private:
  float acceleration = 10.0f;
};
