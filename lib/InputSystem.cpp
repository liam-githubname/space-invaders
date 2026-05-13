// NOTE:===================================================================
// 1. Grab the keyboard state.
// 2. Alter/update the intent components e.g. the PlayerInput struct.
// 3. Apply these intent variables to components of constituient entities.
// 4. The InputSystem explicitly requires the Caller to also call
// SDL_PollEvent(&event), otherwise the keyboard_state will never update.
// TODO:===================================================================
// 6. Input checking for mouse events?
// ========================================================================
#include "InputSystem.hpp"
#include "GameState.hpp"
#include <SDL3/SDL.h>

class InputSystem;

// Public initializer (that calls a private constructor)
InputSystem InputSystem::create() {
  const bool *state = nullptr;
  state = SDL_GetKeyboardState(nullptr);

  return InputSystem(state);
};

// Private Constructor
InputSystem::InputSystem(const bool *keyboard_state)
    : keyboard_state(keyboard_state) {}

void InputSystem::Update(GameState &game_state) {
  auto move_y = 0.0f;
  auto move_x = 0.0f;
  auto fire_input = false;

  if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP]) {
    move_y = -1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) {
    move_x = -1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) {
    move_y = 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) {
    move_x = 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_SPACE]) {
    fire_input = true;
    SDL_Log("is_firing: %d", fire_input);
  }

  // PlayerInput component is updating.
  for (auto &entity : game_state.entities) {
    if (entity.player_input) {
      entity.player_input->move_y = move_y;
      entity.player_input->move_x = move_x;
      entity.player_input->is_firing = fire_input;
      entity.gun->fire_flag = fire_input;
    }
  }
}
