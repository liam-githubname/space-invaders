#pragma once

#include "GameMechanics.hpp"
#include <cstdint>

class GameState;

class MovementSystem {
public:
  void Update(GameState &state, uint64_t deltatime);
};
