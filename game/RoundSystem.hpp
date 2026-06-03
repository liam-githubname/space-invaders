#pragma once
#include "EntityFactory.hpp"
#include "RenderSystem.hpp"
#include "Util.hpp"
class GameState;
struct SDL_Renderer;
class RoundSystem {
public:
  void Update(GameState &game_state, RenderSystem &render_system, EntityFactory &entity_factory,
              const AssetManager &asset_manager, SDL_Renderer *renderer, bool &is_running);

private:
  bool new_round_flag_ = false;
};
