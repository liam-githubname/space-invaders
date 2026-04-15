#include "GameMechanics.hpp"
#include "GraphicsModule.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include <cstdint>
#include <iostream>
#include <string>

//-------------------------------------------------------------------------------
struct timeStep {
  // TODO: I would like to consider using the chrono library instead, but that
  // is a later problem
  // TODO: Write up how this works
  // target_deltatime_nanoseconds represents approximately 1/60th of second.
  static constexpr uint64_t target_deltatime_nanoseconds = 1000000000ULL / 60;
  // accumulator is used to keep the process from spiraling.
  uint64_t accumulator = 0;
  // this is a cursor or time stamp of the previous call to tick()
  uint64_t last_time = 0;
  // Updates the accumulator and last time based on the elapsed time from the
  // start of the program.
  void Tick() {
    uint64_t current_time = SDL_GetTicksNS();
    // this is the time between tick() calls
    uint64_t frame_time = current_time - last_time;
    // NOTE: This is supposed to prevent the spiral of death.
    // If the time since the last tick is greater than
    if (frame_time > 250000000ULL) {
      frame_time = 250000000ULL;
    }
    last_time = current_time;
    accumulator += frame_time;
  }
  // Consumes a step when the tick function has ticked past a the target time
  bool consumeStep() {
    if (accumulator >= target_deltatime_nanoseconds) {
      accumulator -= target_deltatime_nanoseconds;
      return true;
    }
    return false;
  }
  float GetAlpha() {
    return (float)accumulator / (float)target_deltatime_nanoseconds;
  }
};
//---------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  // NOTE:==================== TODO LIST ======================================
  // 1. [x] I want to add the player entity and see that the position exists
  // 2. [~] I want the movement system implement and have the position component
  // update
  // 2.1 I need to build an input system that takes in user input and stores
  // that as a component of player entities.
  // 3. I want to render the player
  // ==========================================================================

  // This is resource Acquisition plus the GraphicsModule object is wrapped
  // in an expected type.
  auto title = std::string("Test Title");
  auto engineResult = GraphicsModule::create(title, 680, 420);
  if (!engineResult.has_value()) {
    SDL_Log("Engine failed to start: %s", engineResult.error().c_str());
    return 1;
  }
  // This unwraps the Graphics Module
  // The program needs to take ownership of the module
  // We have to use std::move because we removed the copy constructor.
  // This is the initialization.
  GraphicsModule graphics = std::move(engineResult.value());

  auto state = GameState();

  // I was wondering how you remember which entities are which
  // You'll remember which entities are which because you should keep handlers
  Entity &player = state.CreateEntity();
  // This emplace doesn't create temporary values? That's pretty cool
  player.is_active = true;
  player.velocity.emplace(0.0f, 0.0f);
  player.transform.emplace(0.0f, 0.0f);
  // how to add an optional field to a struct

  bool isRunning = true;
  SDL_Event event;
  SDL_zero(event);
  InputSystem input_system = InputSystem::create();
  MovementSystem movement_system = MovementSystem();
  timeStep time = timeStep();
  constexpr float dt = 1.0f / 60.0f;

  while (isRunning) {
    // 1. ===================== BEGINNING_OF_INPUT_PROCESSING =================
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        isRunning = false;
    }
    // 1. ==================== END_OF_INPUT_PROCESSING ========================

    time.Tick();

    while (time.consumeStep()) {
      // 2. ==================== UPDATING_GAME_LOGIC =======================
      std::cout << "-" << std::endl;
      input_system.Update(state);
      movement_system.Update(state, dt);

      // 2. ===================== END_OF_GAME_LOGIC ========================
    }

    // 3. ============BEGINNING_OF_RENDER ===============

    // This sets the draw color to white I want to see if there is a better
    // way of doing this
    SDL_SetRenderDrawColor(graphics.getRenderer(), 255, 255, 255, 255);
    // clears the render buffer and fills it with the draw color.
    SDL_RenderClear(graphics.getRenderer());
    // Sets the draw color to red
    SDL_SetRenderDrawColor(graphics.getRenderer(), 0, 0, 0, 255);
    SDL_RenderPresent(graphics.getRenderer());

    // 3. ==================== END_OF_RENDER =================================

    // 4. ================= Manual frame watching MAYBE ====================
    // 4. ================= END_OF_MANUAL_FRAME ====================
  }
  return 0;
}
// const int CIRCLE_DRAW_SIDES = 32;
// const int CIRCLE_DRAW_SIDES_LEN = (CIRCLE_DRAW_SIDES + 1);
// //
// static void drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
//   float ang;
//   SDL_FPoint points[CIRCLE_DRAW_SIDES_LEN];
//   int i;
//   for (i = 0; i < CIRCLE_DRAW_SIDES_LEN; i++) {
//     ang = 2.0f * SDL_PI_F * (float)i / (float)CIRCLE_DRAW_SIDES;
//     points[i].x = x + r * SDL_cosf(ang);
//     points[i].y = y + r * SDL_sinf(ang);
//   }
//   SDL_RenderLines(renderer, (const SDL_FPoint *)&points,
//   CIRCLE_DRAW_SIDES_LEN);
// }
// //
// struct Circle {
//   SDL_FPoint center;
//   float radius;
//   SDL_Texture *texture = NULL; // The reason we use a raw pointer is that
//   // shouldn't care what happens to the texture
//   // It only needs to know where the texture it
//   // must display is located.
//   // We would call this simple observation or something else
// };
//
// Circle createCircle(float x, float y, float r, SDL_Texture *texture) {
//   return Circle{.center = {x, y}, .radius = r, .texture = texture};
// }
//
// struct Splash {
//   Circle ripple;
//   void expandRipple() { ripple.radius += 2.0f; };
// };
//
// Splash createSplash() {
//   return Splash{
//       createCircle(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0.0f,
//       NULL)};
// }

//-------------------------------------------
// if (!splashes.empty()) {
//   if (splashes[0].ripple.radius > 500.0f) {
//     splashes.pop_back();
//   } else {
//     for (auto &splash : splashes) {
//       splash.ripple.radius += 2.5f;
//     }
//   }
// }
//--------------------------------------------
