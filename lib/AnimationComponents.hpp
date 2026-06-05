#pragma once

#include <optional>
#include <unordered_map>
#include "GraphicsModule.hpp"

struct SpriteData {
  SDL_FRect frame1;
  std::optional<SDL_FRect> frame2;
};
struct Sprite {
  SpriteData frame_data;
  bool step_1 = true;
};
struct SpriteSheet {
  TexturePtr sprite_sheet;
  std::unordered_map<std::string, SDL_Rect> coordinate_map;
};
