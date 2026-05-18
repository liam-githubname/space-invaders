/* main.cpp Entry point for game.
 * Authored by Liam Harvell
 */

// NOTE:=======================================================================
// 1. Guide for how logic should flow: -> Mutate state -> Detect facts
// -> Queue facts -> Interpret facts and mutate state again -> Render
// TODO:=======================================================================
// 1. Fix the walls transforms location.
// 2. Move the AssetManager struct to it's own header file.
// 3. Write a utility that pulls the corners of the rectangle colliders.
// 4. Trace the actual worstcase runtime for the program.
// FIX:========================================================================
// 1. The wall entities need to be fixed somehow, it is an absolute nightmare
//    the way it works now. It also causes magic numbers in the collision code.
// ============================================================================

#include "Game.hpp"
#include <SDL3_image/SDL_image.h>

// FIXME: This should probably exist in the game mechanics source file
// but not in the class. That way they can see eachother, but aren't entirely
// coupled.
//-----------------------------------------------------------------------------
// using wrappedTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
// struct AssetManager {
//   void loadTexture(std::string path) {
//     SDL_Texture *raw_texture = IMG_LoadTexture(renderer, path.c_str());
//     textures[path] = wrappedTexture(raw_texture);
//   }
// };
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  auto game = Game::create("Space Invaders", 1920, 1080);
  game->run();
  return 0;
}
