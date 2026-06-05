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
#include "GameConfig.hpp"
#include "GameState.hpp"
#include "Util.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <ranges>

class InputSystem;

// Public initializer (that calls a private constructor)
InputSystem InputSystem::create() {
  const bool *state = nullptr;
  state = SDL_GetKeyboardState(nullptr);

  return InputSystem(state);
};

void update_player_input(const bool *keyboard_state, GameState &game_state) {
  Vec2 move = Zero();
  bool fire_input = false;

  if (keyboard_state[SDL_SCANCODE_W] || keyboard_state[SDL_SCANCODE_UP]) {
    move.y += -1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_A] || keyboard_state[SDL_SCANCODE_LEFT]) {
    move.x += -1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_S] || keyboard_state[SDL_SCANCODE_DOWN]) {
    move.y += 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_D] || keyboard_state[SDL_SCANCODE_RIGHT]) {
    move.x += 1.0f;
  }
  if (keyboard_state[SDL_SCANCODE_SPACE]) {
    fire_input = true;
  }

  auto player_entity = std::find_if(game_state.entities.begin(), game_state.entities.end(),
                                    [](Entity &entity) { return entity.player_input.has_value(); });
  if (player_entity == game_state.entities.end()) {
    SDL_Log("No Player found!");
    return;
  }
  // This lambda expression is a good example for explanation.
  // 1. The [] capture clause - tells compiler that a lambda is beginning
  // It also passes outside variables that the lambda is allowed to see.
  // Empty means there it can see nothing / & means it can see and modify
  // everything.
  // 2. () parameters, it works the same way as always.
  // 3. The & creates a reference instead of a copy which is vital.
  // 4. The body of the lambda is what is ran on all the Entity &entity it
  // finds.
  player_entity->player_input->move = move;
  player_entity->player_input->is_firing = fire_input;
  player_entity->gun->fire_flag = fire_input;
}

void update_alien_input(GameState &game_state) {
  // filtering lambda
  const auto is_alien = [](Entity &entity) { return entity.alien_info.has_value(); };

  for (Entity &alien : game_state.entities | std::views::filter(is_alien)) {
    if (alien.gun.has_value()) {
      alien.gun->fire_flag = (one_in_x(GameConfig::ALIEN_FIRE_CHANCE_1IN) == 1);
    }
  }
}

void InputSystem::Update(GameState &game_state) const {
  update_player_input(keyboard_state, game_state);
  update_alien_input(game_state);
}
// Private Constructor
InputSystem::InputSystem(const bool *keyboard_state) : keyboard_state(keyboard_state) {}
