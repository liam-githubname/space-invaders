#include "GameMechanics.hpp"
#include "GraphicsModule.hpp"

class GameState;
class SDL_Renderer;

// NOTE:===================================================
//
class RenderSystem {
public:
  void Update(GameState &state, SDL_Renderer *renderer);

private:
  void drawCircle(SDL_Renderer renderer, Entity entity);
  void drawPlayer(SDL_Renderer *renderer, Entity player_entitiy);
  void drawTriangle(SDL_Renderer renderer, Entity entity);
  void drawAsteroid(SDL_Renderer renderer, Entity entity);
  void drawScore(SDL_Renderer renderer, Entity entity);
  void drawLives(SDL_Renderer renderer, Entity entity);
  void renderBackground(SDL_Renderer renderer, Entity background);
};
// const int CIRCLE_DRAW_SIDES = 32;
// const int CIRCLE_DRAW_SIDES_LEN = (CIRCLE_DRAW_SIDES + 1);
// //
// static void drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
//
//   SDL_FPoint points[CIRCLE_DRAW_SIDES_LEN];
//   int i;
//   for (i = 0; i < CIRCLE_DRAW_SIDES_LEN; i++) {
//     ang = 2.0f * SDL_PI_F * (float)i / (float)CIRCLE_DRAW_SIDES;
//     points[i].x = x + r * SDL_cosf(ang);
//     points[i].y = y + r * SDL_sinf(ang);
//   }
//   SDL_RenderLines(renderer, (const SDL_FPoint *)&points,
//   CIRCLE_DRAW_SIDES_LEN);
// }
// //
// struct Circle {
//   SDL_FPoint center;
//   float radius;
//   SDL_Texture *texture = NULL; // The reason we use a raw pointer is that
//   // shouldn't care what happens to the texture
//   // It only needs to know where the texture it
//   // must display is located.
//   // We would call this simple observation or something else
// };
//
// Circle createCircle(float x, float y, float r, SDL_Texture *texture) {
//   return Circle{.center = {x, y}, .radius = r, .texture = texture};
// }
//
// struct Splash {
//   Circle ripple;
//   void expandRipple() { ripple.radius += 2.0f; };
// };
//
// Splash createSplash() {
//   return Splash{
//       createCircle(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0.0f,
//       NULL)};
// }

//-------------------------------------------
// if (!splashes.empty()) {
//   if (splashes[0].ripple.radius > 500.0f) {
//     splashes.pop_back();
//   } else {
//     for (auto &splash : splashes) {
//       splash.ripple.radius += 2.5f;
//     }
//   }
// }
//--------------------------------------------
