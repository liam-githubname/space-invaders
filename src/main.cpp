/* main.cpp Entry point for game.
 * Authored by Liam Harvell
 */

// NOTE:=======================================================================
// 1. Guide for how logic should flow: -> Mutate state -> Detect facts
// -> Queue facts -> Interpret facts and mutate state again -> Render
// TODO:=======================================================================
// 2. Move the AssetManager struct to it's own header file.
// 3. Write a utility that pulls the corners of the rectangle colliders.
// 4. Trace the actual worstcase runtime for the program.
// 5. Look into investing into map to hold index for entities.
// FIX:========================================================================
// ============================================================================

#include "Game.hpp"
#include <SDL3_image/SDL_image.h>

int main(int argc, char *argv[]) {
  auto game = Game::create("Space Invaders", 224, 256);
  game->run();
  return 0;
}
