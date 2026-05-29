
#include "RoundSystem.hpp"
#include "EntityFactory.hpp"
#include "GameState.hpp"
#include "RenderSystem.hpp"
#include "SDL3/SDL_render.h"

void RoundSystem::Update(GameState &game_state, RenderSystem &render_system,
                         EntityFactory &entity_factory,
                         SDL_Renderer *renderer) {

  if (new_round_flag_ == true) {

    // FIX: This is a magic operation, the rare evolution of a magic number!
    int i = 0;
    while (i < 100) {
      render_system.Update(game_state, renderer);
      i++;
    }

    entity_factory.createAlienFormation(new_round_offset_position_);

    new_round_flag_ = false;
  }

  if (game_state.number_of_aliens == 0) {
    new_round_flag_ = true;
    game_state.number_of_aliens = 55;
  }
}
