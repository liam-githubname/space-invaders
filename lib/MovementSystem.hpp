#pragma once

#include "GameState.hpp"
#include <cstdint>

class GameState;

class MovementSystem {
public:
  void Update(GameState &game_state, float delta_time);
};
