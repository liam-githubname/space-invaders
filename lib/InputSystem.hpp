#pragma once

#include "GameState.hpp"
#include <SDL3/SDL.h>

class GameState;
// Do I want remove the copy constructor of this class as well?
class InputSystem {

public:
  // This is that public initializer
  static InputSystem create();

  void Update(GameState &game_state);

private:
  const bool *keyboard_state;
  // This is a private constructor that I will have a public initializer call
  InputSystem(const bool *keyboard_state);
};
