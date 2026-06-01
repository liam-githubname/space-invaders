// NOTE:=======================================================================
// TODO:=======================================================================
// FIX:========================================================================
// #1 I need to add a custom destructor for AssetManager because the field:font
// doesn't get destroyed.
// ============================================================================
#pragma once

#include "GameState.hpp"
#include "GraphicsModule.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <unordered_map>
//
// // FIXME: This should probably exist in the game mechanics source file
// // but not in the class. That way they can see eachother, but aren't entirely
// // coupled.
// //-----------------------------------------------------------------------------
//
class SDL_Texture;
struct TTF_Font;

using TexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;

class AssetManager {
public:
  void loadTexture(GraphicsModule &graphics, GameState &game_state);
  SDL_FRect GetTexture() const;
  bool HasTexture(const std::string &key) const;
  std::unordered_map<std::string, SpriteData> textures;
  void createScoreText(GraphicsModule &graphics, GameState &game_state);
  void createScoreNumber(GraphicsModule &graphics, GameState &game_state);
  void createLiveText(GraphicsModule &graphics, GameState &game_state);
  void createLivesCounter(GraphicsModule &graphics, GameState &game_state);
  void createMenuTextures(GraphicsModule &graphics, GameState &game_state);
  void loadFont();
  void Initialize(GraphicsModule &graphics, GameState &game_state);
  TTF_Font *font_ = nullptr;

private:
  // https://www.spriters-resource.com/arcade/spaceinv/asset/115520/ Thanks to
  // spriters-resource for the space invaders sheet.
  std::string file_path_ = "assets/space_invaders_sprite_sheet.png";
  std::string font_path_ = "assets/fonts/space_invaders.ttf";
};
//-----------------------------------------------------------------------------
