#include "GameRules.hpp"
//
// void GameRules::HandleCollisionPayload(const CollisionPayload &payload,
//                                        GameState &game_state) {
//   // Entity pointer holds the address of an entity
//   Entity *entity_a = nullptr;
//   Entity *entity_b = nullptr;
//   // Handle Wall Collisions
//   // ==================================================== Searches through
//   the
//   // entities to find the ones from the payload.
//   for (auto &entity : game_state.entities) {
//     if (entity.id == payload.entity_a_id) {
//       // The address of the entity reference or the address of the entity
//       entity_a = &entity;
//     }
//     if (entity.id == payload.entity_b_id) {
//       entity_b = &entity;
//     }
//   } // WARN: There is a c++ function called find_if that I could use to have
//   // The dereferenced entity pointer should hold a entity reference
//   WallCollisionHandler(*entity_a, *entity_b);
//   //
//   ==========================================================================
// }
//
// void GameRules::WallCollisionHandler(Entity &entity_a, Entity &entity_b) {
//   // Check to see if it's a wall and something with velocity.
//   bool a_velocity = entity_a.velocity.has_value();
//   bool b_velocity = entity_b.velocity.has_value();
//   bool a_wall = entity_a.wall_info.has_value();
//   bool b_wall = entity_b.wall_info.has_value();
//   if (!(a_velocity && b_wall) && !(b_velocity && a_wall))
//     return;
//
//   // This is the easiest way to assign the player.
//   auto &entity = (a_velocity) ? entity_a : entity_b;
//   // This is redundant but a more elegant solution isn't coming to mind rn
//   auto &wall = (a_wall) ? entity_a : entity_b;
//
//   float wall_x, wall_y;
//   WallSide wallside;
//
//   // Set the velocity to zero
//   // player.velocity->dy = 0.0f;
//   // player.velocity->dx = 0.0f;
//
//   wall_x = wall.transform->x + wall.collider->offset_x;
//   wall_y = wall.transform->y + wall.collider->offset_y;
//   wallside = wall.wall_info->side;
//
//   switch (wallside) {
//   case WallSide::Top:
//     entity.transform->y = wall_y + entity.collider->rect.height / 2;
//     break;
//   case WallSide::Bottom:
//     entity.transform->y = wall_y - entity.collider->rect.height / 2;
//     break;
//   case WallSide::Left:
//     entity.transform->x = wall_x + entity.collider->rect.width / 2;
//     break;
//   case WallSide::Right:
//     entity.transform->x = wall_x - entity.collider->rect.width / 2;
//     break;
//   default:
//     SDL_Log("in default switch case in EventSystem.cpp");
//     break;
//   }
//
//   // If an alien hits the wall add the altermovement component
//   // So I went looking to see if C++ had the ability to filter a vector with
//   a
//   // lambda and then to iterate over it and it does!
//   // Create a Predicate (lambda expression)
//   auto is_alien = [](Entity entity) {
//     switch (entity.alien_info->type) {
//     case AlienSpecies::Squid:
//       return true;
//       break;
//     case AlienSpecies::Crab:
//       return true;
//       break;
//     case AlienSpecies::Octopus:
//       return true;
//       break;
//     default:
//       return false;
//     }
//   };
//
//   for (auto &enemy :)
//     if (!entity.movement_mod.has_value()) {
//       entity.movement_mod.emplace(AlterMovement{
//           .speed_mod = -1.0,
//       });
//       entity.transform->y += 100.0f;
//     } else {
//       entity.movement_mod->speed_mod *= -1.0;
//       entity.transform->y += 100.0f;
//     }
// }
