#include "InputSystem.hpp"
#include "GameState.hpp"
#include <SDL3/SDL.h>
#include <iostream>

class InputSystem;

// Public initializer (that calls a private constructor)
InputSystem InputSystem::create() {
  const bool *state = nullptr;
  state = SDL_GetKeyboardState(nullptr);

  return InputSystem(state);
};

// Private constructor
InputSystem::InputSystem(const bool *keyboard_state)
    : keyboard_state(keyboard_state) {}

void InputSystem::Update(GameState &game_state) {
  auto velocity_adder = 0.0f;

  // NOTE:===================================================================
  // 1. Grab the keyboard state
  // 2. Alter/update the intent variables
  // 3. Apply these intent variables to components of constituient entities.
  // ========================================================================

  // What is the right way to reset this value?
  if (keyboard_state[SDL_SCANCODE_W]) {
    velocity_adder = 200.0f;
  }

  for (auto &entity : game_state.entities) {
    if (entity.is_active && entity.transform.has_value()) {
      entity.velocity->dy = velocity_adder;
      entity.velocity->dx = velocity_adder;
    }
  }
}
