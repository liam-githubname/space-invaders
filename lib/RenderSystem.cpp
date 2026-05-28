#include "RenderSystem.hpp"
#include "Bitmask.hpp"
#include "GameState.hpp"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class RenderSystem;

void RenderSystem::Update(GameState &game_state, SDL_Renderer *renderer) {

  // This sets the draw color to white I want to see if there is a better
  // way of doing this
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  // clears the render buffer and fills it with the draw color.
  SDL_RenderClear(renderer);

  // NOTE: I am not making entity a reference because it doesn't need to affect
  // it.
  for (auto &entity : game_state.entities) {
    if (!entity.is_active)
      continue;

    if (entity.sprite.has_value()) {

      // calculate the position the sprite needs to be in.
      const auto destination =
          SDL_FRect{.x = entity.transform->position.x -
                         entity.sprite->frame_data.frame1.w / 2,
                    .y = entity.transform->position.y -
                         entity.sprite->frame_data.frame1.h / 2,
                    .w = entity.sprite->frame_data.frame1.w,
                    .h = entity.sprite->frame_data.frame1.h};

      // I don't like that the sprite component has a different syntax to access
      // frame1 and frame2 fields
      auto &sprite_coordinates = (entity.sprite->step_1)
                                     ? entity.sprite->frame_data.frame1
                                     : entity.sprite->frame_data.frame2.value();

      SDL_RenderTexture(renderer, game_state.textures.sprite_sheet.get(),
                        &sprite_coordinates, &destination);
    }

    // if (entity.bitmask->layer == GameLayer::Player) {
    //   drawRectangle(renderer, entity);
    //   // drawFire(renderer, entity);
    // }
    //
    // if (entity.bitmask->layer == GameLayer::Projectile) {
    //   drawRectangle(renderer, entity);
    // }
    //
    // if (entity.bitmask->layer == GameLayer::Enemy) {
    //   drawRectangle(renderer, entity);
    // }
    //
    // if (entity.bitmask->layer == GameLayer::Wall) {
    //   drawWall(renderer, entity);
    // }
  }

  // This puts all of the rendering into the window
  //  1. I pulled this from the main loop to remove the rendering from the main
  //  loop.

  if (game_state.score_board_texture != nullptr) {
    SDL_FRect score_title_rect{5, 1, (float)game_state.score_board_texture->w,
                               (float)game_state.score_board_texture->h};
    SDL_RenderTexture(renderer, game_state.score_board_texture, NULL,
                      &score_title_rect);
  }

  if (game_state.score_texture != nullptr) {

    SDL_FRect score_rect{40, 1, (float)game_state.score_texture->w,
                         (float)game_state.score_texture->h};
    SDL_RenderTexture(renderer, game_state.score_texture, NULL, &score_rect);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderPresent(renderer);
}

void RenderSystem::drawRectangle(SDL_Renderer *renderer, Entity entity) {
  // This was the first time I used this syntax instinctually
  SDL_FRect player_rect{
      entity.transform->position.x - entity.collider->rect.width / 2,
      entity.transform->position.y - entity.collider->rect.height / 2,
      entity.collider->rect.width, entity.collider->rect.height};
  // SDL_FRect player_center{entity.transform->position.x,
  //                         entity.transform->position.y, 5.0, 5.0};
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderRect(renderer, &player_rect);
  // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  // SDL_RenderFillRect(renderer, &player_center);
}

void RenderSystem::drawWall(SDL_Renderer *renderer, Entity wall) {
  SDL_FRect wall_rect{
      wall.transform->position.x - wall.collider->rect.width / 2,
      wall.transform->position.y - wall.collider->rect.height / 2,
      wall.collider->rect.width, wall.collider->rect.height};
  // SDL_FRect wall_center{
  //     wall.transform->position.x - wall.collider->rect.width / 2,
  //     wall.transform->position.y - wall.collider->rect.height / 2, 2.0, 2.0};
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, &wall_rect);
  // SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  // SDL_RenderFillRect(renderer, &wall_center);
}

void RenderSystem::drawFire(SDL_Renderer *renderer, Entity entity) {

  if (!entity.gun->fire_flag)
    return;

  auto endpointx = entity.transform->position.x +
                   entity.transform->direction.x * entity.gun->distance;
  auto endpointy = entity.transform->position.y +
                   entity.transform->direction.y * entity.gun->distance;

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderLine(renderer, entity.transform->position.x,
                 entity.transform->position.y, endpointx, endpointy);
}
