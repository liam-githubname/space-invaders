#pragma once

#include "EntityFactory.hpp"
#include "RenderSystem.hpp"
#include "Util.hpp"
class GameState;
struct SDL_Renderer;
class RoundSystem {
public:
  void Update(GameState &game_state, RenderSystem &render_system,
              EntityFactory &entity_factory, SDL_Renderer *renderer);

private:
  Vec2 new_round_offset_position_ = Vec2{.x = 0.0f, .y = 8.0f};
  bool new_round_flag_ = false;
};
