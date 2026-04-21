#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <cstddef>
#include <expected>
#include <memory>
#include <string_view>

// TODO: I need to add a fixed time step for game logic processing I don't know
// where I need to implement that though.
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

// These need to become private methods of a graphics class
using TexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;
using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;

class GraphicsModule {
public:
  // Important facts about std::expected
  // 1. It is based on Rust Result type.
  // 2. Don't have to throw exceptions anymore.
  static std::expected<GraphicsModule, std::string>
  // std::string_view is a pointer and a length, it is essentially a reference
  // to a string. It is much faster than a string because it is allocation free.
  create(std::string_view title, int width, int height);

  // Delete copy semantics
  GraphicsModule(const GraphicsModule &) = delete;
  GraphicsModule &operator=(const GraphicsModule &) = delete;

  // 4. Allow Move Semantics (so we can return it from the factory)
  GraphicsModule(GraphicsModule &&) = default;
  GraphicsModule &operator=(GraphicsModule &&) = default;

  ~GraphicsModule();

  SDL_Window *getWindow() const { return window_.get(); }
  SDL_Renderer *getRenderer() const { return renderer_.get(); }

private:
  GraphicsModule(SDL_Window *window, SDL_Renderer *renderer);

  std::unique_ptr<SDL_Window, WindowDeleter> window_;
  std::unique_ptr<SDL_Renderer, RendererDeleter> renderer_;
};

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
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
