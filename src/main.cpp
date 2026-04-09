#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <memory>
#include <print>
#include <random>
#include <sdl3/SDL.h>
#include <sdl3/SDL_main.h>

// The SDL Subsystem RAII wrapper, SDL_Init must be called before anything else
// and SDL_Quit must be called after the game closes
// We will tie this to the lifecycle of a simple struct.
// I really like this idea, you can tie necessary behaviors to a related
// overarching type/object

// Is this a preprocessor command or some kind of runtime compilation?
// [[nodiscard]] std::expected<EngineCore, std::string>
// initialize_engine(const char *title, int width, int height) {
//   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
//     return std::unexpected(std::string("SDL_Init Error") + SDL_GetError());
//   }
//
//   // Initialize the window and then assign it to our smart pointer.
//   WindowPtr window{SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
//                                     SDL_WINDOWPOS_CENTERED, width, height,
//                                     SDL_WINDOW_SHOWN)};
//
//   RendererPtr renderer{SDL_CreateRenderer(
//       window.get(), -1, SDL_RENDERER_ACCELERATED |
//       SDL_RENDERER_PRESENTVSYNC)};
//
//   return EngineCore{SDLContext{}, std::move(window), std::move(renderer)};
// }

// ============= This might need be used again for a rendering module? =========
const int CIRCLE_DRAW_SIDES = 32;
const int CIRCLE_DRAW_SIDES_LEN = (CIRCLE_DRAW_SIDES + 1);

static void drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
  float ang;
  SDL_FPoint points[CIRCLE_DRAW_SIDES_LEN];
  int i;
  for (i = 0; i < CIRCLE_DRAW_SIDES_LEN; i++) {
    ang = 2.0f * SDL_PI_F * (float)i / (float)CIRCLE_DRAW_SIDES;
    points[i].x = x + r * SDL_cosf(ang);
    points[i].y = y + r * SDL_sinf(ang);
  }
  SDL_RenderLines(renderer, (const SDL_FPoint *)&points, CIRCLE_DRAW_SIDES_LEN);
}

struct Circle {
  SDL_FPoint center;
  float radius;
  SDL_Texture *texture = NULL; // The reason we use a raw pointer is that Circle
                               // shouldn't care what happens to the texture
                               // It only needs to know where the texture it
                               // must display is located.
  // We would call this simple observation or something else
};

Circle createCircle(float x, float y, float r, SDL_Texture *texture) {
  return Circle{.center = {x, y}, .radius = r, .texture = texture};
}

int main(int argc, char *argv[]) {
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::random_device rand;
  std::mt19937 gen(rand());
  std::uniform_real_distribution<float> radius_dis(10.0f, 100.0f);
  std::uniform_real_distribution<float> pos_x_dis(100.0f, 540.0f);
  std::uniform_real_distribution<float> pos_y_dis(100.0f, 380.0f);
  std::uniform_real_distribution<float> color_dis(0.0f, 1.0f);
  std::vector<Circle> circles;
  if (!SDL_CreateWindowAndRenderer("examples/demo/woodeneye-008", 640, 480,
                                   SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    std::println("Failed");
    return SDL_APP_FAILURE;
  }

  TexturePtr bjorkTexture(IMG_LoadTexture(renderer, "assets/bjork.jpeg"));
  if (!bjorkTexture) {
    std::println(stderr, "Failed to load the big bjork");
  }

  std::println("Main has started actually");
  SDL_Event event;
  bool run = true;

  while (run) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
          run = false;
        }

        if (event.key.key == SDLK_SPACE) {
          circles.push_back(createCircle(pos_x_dis(gen), pos_y_dis(gen),
                                         radius_dis(gen), bjorkTexture.get()));
        }
      }

      if (event.type == SDL_EVENT_QUIT) {
        run = false;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (const auto &circle : circles) {
      SDL_FRect destination = {circle.center.x - circle.radius,
                               circle.center.y - circle.radius,
                               circle.radius * 2, circle.radius * 2};

      SDL_RenderTexture(renderer, circle.texture, NULL, &destination);

      drawCircle(renderer, circle.radius, circle.center.x, circle.center.y);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_Quit();
  return 0;
}
