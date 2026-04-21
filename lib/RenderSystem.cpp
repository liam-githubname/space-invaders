#include "RenderSystem.hpp"
#include "GameState.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

struct AssetManager;
class RenderSystem;

// FIXME: Replace this with rendering an actual texture.
// 1. I need to figure out how to render directions as well.
void RenderSystem::drawPlayer(SDL_Renderer *renderer, Entity player_entity) {
  // This was the first time I used this syntax instinctually
  SDL_FRect player_rect{player_entity.transform->x, player_entity.transform->y,
                        100.0, 100.0};
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderRect(renderer, &player_rect);
}

//-----------------------------------------------------------------------------
void RenderSystem::Update(GameState &game_state, SDL_Renderer *renderer) {
  // TODO: Add updates the game rendering as I have new features come online

  // This sets the draw color to white I want to see if there is a better
  // way of doing this
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  // clears the render buffer and fills it with the draw color.
  SDL_RenderClear(renderer);

  // NOTE: I am not making entity a reference because it doesn't need to affect
  // it.
  for (auto entity : game_state.entities) {

    if (entity.is_player.has_value()) {
      drawPlayer(renderer, entity);
    }
  }

  // This puts all of the rendering into the window
  //  1. I pulled this from the main loop to remove the rendering from the main
  //  loop.
  SDL_RenderPresent(renderer);
}
