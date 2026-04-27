// NOTE:===================================================================
// 1. Grab the keyboard state.
// 2. Alter/update the intent components e.g. the PlayerInput struct.
// 3. Apply these intent variables to components of constituient entities.
// 4. The InputSystem explicitly requires the Caller to also call
// SDL_PollEvent(&event), otherwise the keyboard_state will never update.
// TODO:===================================================================
// 1. The input system applies velocity to all entities.
// 4. Make it update input state components on player character.
// 5. Input checking for space bar
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
  auto is_firing = false;

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
    is_firing = true;
    SDL_Log("is_firing: %d", is_firing);
  }

  // PlayerInput component is updating.
  for (auto &entity : game_state.entities) {
    if (entity.is_player.has_value() && entity.player_input.has_value()) {
      entity.player_input->move_y = move_y;
      entity.player_input->move_x = move_x;
      entity.player_input->is_firing = is_firing;
    }
  }
}
