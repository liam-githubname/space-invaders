
#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <memory>
#include <print>
#include <random>
#include <sdl3/SDL.h>
#include <sdl3/SDL_main.h>

// We are defining how to destroy SDL resources
struct WindowDeleter {
  void operator()(SDL_Window *window) const {
    if (window)
      SDL_DestroyWindow(window);
  }
};
struct RendererDeleter {
  void operator()(SDL_Renderer *renderer) const {
    if (renderer)
      SDL_DestroyRenderer(renderer);
  }
};
struct SDLTextureDeleter {
  void operator()(SDL_Texture *texture) const {
    if (texture)
      SDL_DestroyTexture(texture);
  }
};

using TexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;

// ======================= This was a first attempt ===========================
struct SDLContext {
  // I'm assuming there is a default initializer or constructor of a struct and
  // we can assign it like this.
  SDLContext() = default;
  ~SDLContext() { SDL_Quit(); }

  // // Rule of Five: Delete copy and move operations to enforce exclusive
  // // ownership This ensures SDL_Init/SDL_Quit are only called once and never
  // // duplicated Without these, copying would cause double SDL_Quit() calls
  // // (undefined behavior) Moving would leave one context calling SDL_Quit()
  // // while another thinks SDL is active This is the standard C++ pattern for
  // // non-copyable/non-movable RAII types
  // SDLContext(const SDLContext &) = delete;
  // SDLContext &operator=(const SDLContext &) = delete;
  // SDLContext(SDLContext &&) = delete;
  // SDLContext &operator=(SDLContext &&) = delete;
};

struct EngineCore {
  SDLContext
      context; // Ensures that SDL_Quit is called when EngineCore is destroyed.
  WindowPtr window;
  RendererPtr renderer;
};
// ======================= This was a first attempt ===========================
