#include "GameState.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>

class GameState;

Entity &GameState::CreateEntity() {
  // Creates new one.
  Entity new_entity;
  // Increments the id of the entity
  new_entity.id = next_id++;
  // pushes it onto game state vector
  entities.push_back(new_entity);
  return entities.back();
};

void GameState::DestroyEntity(uint32_t target_id) {
  // WARN: I believe that &entity must be a reference otherwise it
  // would copy the value (bad for effieciency)
  // wouldn't update the value (because entity is a copy)
  for (auto &entity : entities) {
    if (entity.id == target_id) {
      entity.is_active = false;
      break;
    }
  };
}
class SDL_Texture;
class SDL_TextureDeleter;
class SDL_Renderer;
//-----------------------------------------------------------------------------
using wrappedTexture = std::unique_ptr<SDL_Texture, SDL_TextureDeleter>;
