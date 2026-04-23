#include "CollisionSystem.hpp"
#include "EventSystem.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include "RenderSystem.hpp"
#include <SDL3_image/SDL_image.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

// FIXME: This should probably exist in the game mechanics source file
// but not in the class. That way they can see eachother, but aren't entirely
// coupled.
//-----------------------------------------------------------------------------
// using wrappedTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
// struct AssetManager {
//   // TODO: INSTANTIATE after graphics module and pass the renderer to it
//   std::unordered_map<std::string, wrappedTexture> textures;
//   SDL_Renderer *renderer;
//
//   void loadTexture(std::string path) {
//     SDL_Texture *raw_texture = IMG_LoadTexture(renderer, path.c_str());
//     textures[path] = wrappedTexture(raw_texture);
//   }
// };
//-----------------------------------------------------------------------------

//--------------TIMESTEP_STRUCT_FOR_DETERMINISTIC_BEHAVIOR---------------------
struct TimeStep {
  // TODO: I would like to consider using the chrono library instead, but that
  // is a later problem
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
    uint64_t frame_time = current_time - last_time;
    if (frame_time > 250000000ULL) {
      frame_time = 250000000ULL;
    }
    last_time = current_time;
    accumulator += frame_time;
  }
  // INFO: consumeStep is what allows for "substeps" These are steps that occur
  // within one frame. It is unlikely to ever have more than one substep, but it
  // is a safety precaution.
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
//=============TIMESTEP_STRUCT_FOR_DETERMINISTIC_BEHAVIOR======================

int main(int argc, char *argv[]) {

  // NOTE:==================== TODO LIST ======================================
  // 3. [~] I want to render the player. WE WILL GET BACK TO THIS!
  //   3.1 render a texture for the player.
  //   AND
  //   3.1 [X] draw a shape to represent the player.
  // 4. [X] I need to add bounding and AABB collision detection features.
  //  - AABB will not be enough. But good for now.
  // 5. I need to figure how to rotate a texture.
  // 6. Graphics module needs to be overhauled to support full screen
  // 7. [X] Event queue system needs implementation in EventSystem.*pp
  // 8. [X] Collision detection needs updates in CollisionSystem.*pp
  // 9. [X] Update the CmakeLists.txt to include the event system after I make
  // it
  // 10. Add the collision Decider function
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

  // ==================== Initialization of Systems ===========================
  TimeStep time_step = TimeStep();
  // TODO: Figure out if the other systems should remove some of their default
  // constructors. Like GraphicsModule.
  GraphicsModule graphics = std::move(engineResult.value());
  GameState game_state = GameState();
  SDL_Event event;
  SDL_zero(event);
  InputSystem input_system = InputSystem::create();
  MovementSystem movement_system = MovementSystem();
  CollisionSystem collision_system = CollisionSystem();
  EventSystem event_system = EventSystem();
  RenderSystem render_system = RenderSystem();

  // miscellaneous
  constexpr float dt = 1.0f / 60.0f;
  int window_width, window_height;
  SDL_GetWindowSize(graphics.getWindow(), &window_width, &window_height);
  bool is_running = true;

  // ==================== Initialization of entities ==========================
  // TODO: Look at making CreateEntity a factory pattern?
  Entity &player = game_state.CreateEntity();
  player.is_player.emplace();
  player.is_active = true;
  player.velocity.emplace(0.0f, 0.0f);
  player.transform.emplace(window_width / 2, window_height / 2);
  player.collider.emplace(Collider{.shape = ColliderShape::Rectangle,
                                   .offset_x = 0,
                                   .offset_y = 0,
                                   .rect{100.0, 100.0}});
  std::vector<Entity> walls;

  for (int i = 0; i < 2; i++) {
    auto x_wall = game_state.CreateEntity();
    auto y_wall = game_state.CreateEntity();

    x_wall.is_wall.emplace();
    y_wall.is_wall.emplace();
    // TODO: I would like to use "designated initialization syntax"
    // It was throwing errors though
    x_wall.transform.emplace(0 + i * window_width, 0);
    y_wall.transform.emplace(0, 0 + i * window_height);
    SDL_Log("x_wall coords: %f, %f", x_wall.transform->x, x_wall.transform->y);
    SDL_Log("y_wall coords: %f, %f", y_wall.transform->x, y_wall.transform->y);
    walls.push_back(x_wall);
    walls.push_back(y_wall);
  }
  // INFO: Guide for how logic should flow
  // -> Mutate state -> Detect facts
  // -> Queue facts -> Interpret facts and mutate state again
  // -> Render

  while (is_running) {
    // It is important to realize that the input_system actually relies on this
    // call to SDL_PollEvent to update the keyboard state array
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        is_running = false;
    }
    time_step.Tick();

    while (time_step.consumeStep()) {
      //========================== Input & Logic ==============================
      input_system.Update(game_state);
      //========================== Movement ===================================
      movement_system.Update(game_state, dt);
      //========================== Collision ==================================
      collision_system.Update(game_state);
    }
    //========================== Consume Events ===============================
    event_system.ProcessEvents(game_state);
    //============================ Render =====================================
    render_system.Update(game_state, graphics.getRenderer());
  }
  return 0;
}
