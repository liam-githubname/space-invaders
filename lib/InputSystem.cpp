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
#include <algorithm>

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
    move_y += -1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) {
    move_x += -1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) {
    move_y += 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) {
    move_x += 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_SPACE]) {
    fire_input = true;
  }

  // This is another way to search through something.
  auto player_entity = std::find_if(
      game_state.entities.begin(), game_state.entities.end(),
      [](Entity &entity) { return entity.player_input.has_value(); });
  // This lambda expression is a good example for explanation.
  // 1. The [] capture clause - tells compiler that a lambda is beginning
  // It also passes outside variables that the lambda is allowed to see.
  // Empty means there it can see nothing / & means it can see and modify
  // everything.
  // 2. () parameters, it works the same way as always.
  // 3. The & creates a reference instead of a copy which is vital.
  // 4. The body of the lambda is what is ran on all the Entity &entity it
  // finds.

  player_entity->velocity->dy = move_y * player_entity->velocity->speed;
  player_entity->velocity->dx = move_x * player_entity->velocity->speed;
  // WARN: For space invaders this is something that I don't want to change.
  // I know that that's not how I should do it if I want gameplay to be
  // completely decoupled. I should do something like have a response and update
  // component maybe? entity->transform->direction_y = move_y;
  // entity->transform->direction_x = move_x;
  player_entity->player_input->is_firing = fire_input;
  player_entity->gun->fire_flag = fire_input;
}
