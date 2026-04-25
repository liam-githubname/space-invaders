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
  SDL_FRect player_center{player_entity.transform->x,
                          player_entity.transform->y, 10.0, 10.0};
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderRect(renderer, &player_rect);
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &player_center);
}

void RenderSystem::drawWall(SDL_Renderer *renderer, Entity wall) {
  SDL_FRect wall_rect{wall.transform->x, wall.transform->y,
                      wall.collider->rect.width, wall.collider->rect.height};
  SDL_FRect wall_center{wall.transform->x, wall.transform->y, 2.0, 2.0};
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &wall_rect);
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &wall_center);
}

//-----------------------------------------------------------------------------
void RenderSystem::Update(GameState &game_state, SDL_Renderer *renderer) {
  // TODO: Add updates the game rendering as I have new features come online

  // This sets the draw color to white I want to see if there is a better
  // way of doing this
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  // clears the render buffer and fills it with the draw color.
  SDL_RenderClear(renderer);

  // NOTE: I am not making entity a reference because it doesn't need to affect
  // it.
  for (auto entity : game_state.entities) {

    if (entity.is_player.has_value()) {
      drawPlayer(renderer, entity);
    }

    if (entity.is_wall.has_value()) {
      drawWall(renderer, entity);
    }
  }

  // This puts all of the rendering into the window
  //  1. I pulled this from the main loop to remove the rendering from the main
  //  loop.
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderPresent(renderer);
}
