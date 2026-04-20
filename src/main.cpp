#include "GameMechanics.hpp"
#include "GraphicsModule.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include "RenderModule.hpp"
#include <SDL3_image/SDL_image.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

// FIXME: This should probably exist in the game mechanics source file
// but not in the class. That way they can see eachother, but aren't entirely
// coupled.
//-----------------------------------------------------------------------------
using wrappedTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
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

// NOTE: I feel like this is probably the best way to do this for now.
// I don't need to have a class dedicated to this. Plus it's only ever going to
// used here.
//--------------TIMESTEP_STRUCT_FOR_DETERMINISTIC_BEHAVIOR---------------------
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
//=============TIMESTEP_STRUCT_FOR_DETERMINISTIC_BEHAVIOR======================

int main(int argc, char *argv[]) {

  // NOTE:==================== TODO LIST ======================================
  // 1. [x] I want to add the player entity and see that the position exists
  // 2. [X] I want the movement system implement and have the position component
  // update
  // 2.1 [X] I need to build an input system that takes in user input and stores
  // that as a component of player entities.
  // 3. [~] I want to render the player. WE WILL GET BACK TO THIS!
  //   3.1 render a texture for the player.
  //   AND
  //   3.1 [X] draw a shape to represent the player.
  // 4. I need to add bounding and AABB collision detection features.
  //  - AABB will not be enough. But good for now.
  // 5. I need to figure how to rotate a texture.
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

  // auto asset_manager = AssetManager();
  // asset_manager.renderer = graphics.getRenderer();
  // asset_manager.loadTexture(bg_texture_key);

  auto state = GameState();

  int window_width, window_height;
  SDL_GetWindowSize(graphics.getWindow(), &window_width, &window_height);
  // I was wondering how you remember which entities are which
  // You'll remember which entities are which because you should keep handlers
  Entity &player = state.CreateEntity();
  player.isPlayer.emplace();
  // This emplace doesn't create temporary values? That's pretty cool
  player.is_active = true;
  player.velocity.emplace(0.0f, 0.0f);
  player.transform.emplace(window_width / 2, window_height / 2);
  // how to add an optional field to a struct
  Entity &background = state.CreateEntity();
  // background.sprite.emplace(
  //     Sprite{bg_texture_key,
  //     (float)asset_manager.textures[bg_texture_key]->w,
  //            (float)asset_manager.textures[bg_texture_key]->h});
  // std::cout << "background.sprite.has_value()" <<
  // background.sprite.has_value()
  //           << std::endl;

  bool isRunning = true;
  SDL_Event event;
  SDL_zero(event);
  InputSystem input_system = InputSystem::create();
  MovementSystem movement_system = MovementSystem();
  timeStep time = timeStep();
  constexpr float dt = 1.0f / 60.0f;
  RenderSystem render_system = RenderSystem();

  // # NOTE: This is the basic idea for textures.
  // auto bg_texture_key =
  //     SDL_GetBasePath() + std::string("assets/blue-preview.png");
  // SDL_Texture *bg_text =
  //     IMG_LoadTexture(graphics.getRenderer(), bg_texture_key.c_str());

  while (isRunning) {
    // TODO: Find out if this should be somewhere else maybe
    // It is important to realize that the input_system actually relies on this
    // call to SDL_PollEvent to update the keyboard state array
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        isRunning = false;
    }
    time.Tick();

    while (time.consumeStep()) {
      //========================== Input & Logic ==============================
      input_system.Update(state);
      //========================== Movement ===================================
      movement_system.Update(state, dt);
      //========================== Collision ==================================
      // TODO: Add the collision system's update.
    }
    //============================ Render =====================================
    render_system.Update(state, graphics.getRenderer());
  }
  return 0;
}
