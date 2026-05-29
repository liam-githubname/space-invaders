#include "AssetManager.hpp"
#include "GameState.hpp"
#include "GraphicsModule.hpp"
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

void AssetManager::loadTexture(GraphicsModule &graphics,
                               GameState &game_state) {
  auto *raw_texture =
      IMG_LoadTexture(graphics.getRenderer(), file_path_.c_str());
  if (!raw_texture) {
    SDL_Log("Failed to load sprite sheet");
    return;
  }

  // This sets the way sdl renders this texture. Scales to nearest whole so
  // things don't get blurry
  SDL_SetTextureScaleMode(raw_texture, SDL_SCALEMODE_PIXELART);
  game_state.textures.sprite_sheet.reset(raw_texture);

  textures["squid"] = SpriteData{
      .frame1 = SDL_FRect{.x = 5.0f, .y = 1.0f, .w = 8.0f, .h = 8.0f},
      .frame2 = SDL_FRect{.x = 5.0f, .y = 11.0f, .w = 8.0f, .h = 8.0f},
  };
  textures["crab"] = SpriteData{
      .frame1 = SDL_FRect{.x = 22.0f, .y = 1.0f, .w = 11.0f, .h = 8.0f},
      .frame2 = SDL_FRect{.x = 22.0f, .y = 11.0f, .w = 11.0f, .h = 8.0f},
  };
  textures["octopus"] = SpriteData{
      .frame1 = SDL_FRect{.x = 39.0f, .y = 1.0f, .w = 12.0f, .h = 8.0f},
      .frame2 = SDL_FRect{.x = 39.0f, .y = 11.0f, .w = 12.0f, .h = 8.0f},
  };
  textures["explosion"] = SpriteData{
      .frame1 = SDL_FRect{.x = 56.0f, .y = 1.0f, .w = 13.0f, .h = 8.0f},
  };
  textures["canon"] = SpriteData{
      .frame1 = SDL_FRect{.x = 3.0f, .y = 49.0f, .w = 13.0f, .h = 8.0f},
  };
  textures["spaceship"] = SpriteData{
      .frame1 = SDL_FRect{.x = 1.0f, .y = 40.0f, .w = 16.0f, .h = 7.0f},
  };
  textures["spaceship-death"] = SpriteData{
      .frame1 = SDL_FRect{.x = 20.0f, .y = 39.0f, .w = 21.0f, .h = 8.0f},
  };
  textures["barrier"] = SpriteData{
      .frame1 = SDL_FRect{.x = 46.0f, .y = 31.0f, .w = 22.0f, .h = 16.0f},
  };
  textures["bullet"] = SpriteData{
      .frame1 = SDL_FRect{.x = 55.0f, .y = 53.0f, .w = 1.0f, .h = 4.0f},
  };
}

void AssetManager::loadFont() {
  auto *font = TTF_OpenFont(font_path_.c_str(), 8);
  if (!font) {
    SDL_Log("Failed to load font: %s", SDL_GetError());
  }
  font_ = font;
}

void AssetManager::createScoreText(GraphicsModule &graphics,
                                   GameState &game_state) {

  SDL_Surface *surface = TTF_RenderText_Blended(
      font_, "SCORE", 0, SDL_Color{.r = 255, .g = 255, .b = 255, .a = 255});

  game_state.score_board_texture =
      SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
  SDL_SetTextureScaleMode(game_state.score_board_texture,
                          SDL_SCALEMODE_PIXELART);

  SDL_DestroySurface(surface);
}

void AssetManager::createScoreNumber(GraphicsModule &graphics,
                                     GameState &game_state) {

  SDL_DestroyTexture(game_state.score_texture);
  auto score_str = std::to_string(game_state.score);

  SDL_Surface *surface = TTF_RenderText_Blended(font_, score_str.c_str(), 0,
                                                SDL_Color{0, 255, 0, 255});

  game_state.score_texture =
      SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
  SDL_SetTextureScaleMode(game_state.score_texture, SDL_SCALEMODE_PIXELART);

  SDL_DestroySurface(surface);

  game_state.score_update = false;
}

void AssetManager::createLiveText(GraphicsModule &graphics,
                                  GameState &game_state) {
  SDL_DestroyTexture(game_state.live_text_texture);
  auto lives_str = "lives";

  SDL_Surface *surface = TTF_RenderText_Blended(font_, lives_str, 0,
                                                SDL_Color{255, 255, 255, 255});
  game_state.live_text_texture =
      SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
  SDL_SetTextureScaleMode(game_state.live_text_texture, SDL_SCALEMODE_PIXELART);

  SDL_DestroySurface(surface);
}

void AssetManager::createLivesCounter(GraphicsModule &graphics,
                                      GameState &game_state) {

  SDL_DestroyTexture(game_state.lives_texture);
  float width = 120.0f;
  int number_of_rows = game_state.number_of_lives / 4 + 1;
  float height = (float)number_of_rows * 14.0f;

  // create a texture that should fit all of the lives
  SDL_Texture *lives_texture =
      SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_BGRA8888,
                        SDL_TEXTUREACCESS_TARGET, width, height);

  // some settings to make the background blend in.
  SDL_SetTextureBlendMode(lives_texture, SDL_BLENDMODE_BLEND);

  // turns out this is necessary to have sprite be rendered to the texture as
  // pixelart
  SDL_SetTextureScaleMode(lives_texture, SDL_SCALEMODE_PIXELART);

  // moves the renderers target to the new texture.
  SDL_SetRenderTarget(graphics.getRenderer(), lives_texture);

  // logically places player_sprites according to how many there are
  const auto &player_sprite = textures["canon"];
  for (int i = 0; i < game_state.number_of_lives; i++) {

    auto column_offset = (i % 4) * (player_sprite.frame1.w + 2.0f);

    auto row_offset = (i / 4) * (player_sprite.frame1.h + 1.0f);

    auto dstRect = SDL_FRect{.x = column_offset,
                             .y = row_offset,
                             .w = player_sprite.frame1.w,
                             .h = player_sprite.frame1.h};
    // const because I don't want any chance in altering the sprite data

    SDL_RenderTexture(graphics.getRenderer(),
                      game_state.textures.sprite_sheet.get(),
                      &player_sprite.frame1, &dstRect);
  }

  // resets the target for the renderer.
  SDL_SetRenderTarget(graphics.getRenderer(), nullptr);

  game_state.lives_texture = lives_texture;
  SDL_SetTextureScaleMode(game_state.live_text_texture, SDL_SCALEMODE_PIXELART);
}

void AssetManager::Initialize(GraphicsModule &graphics, GameState &game_state) {
  loadFont();
  loadTexture(graphics, game_state);
  createScoreText(graphics, game_state);
  createScoreNumber(graphics, game_state);
  createLiveText(graphics, game_state);
  createLivesCounter(graphics, game_state);
}
