#include "GraphicsModule.hpp"
#include <expected>
#include <string>

std::expected<GraphicsModule, std::string>
GraphicsModule::create(std::string_view title, int width, int height) {

  if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    return std::unexpected<std::string>(
        std::string("Failed to initialize SDL Video Subsystem") +
        SDL_GetError());
  }

  SDL_Window *rawWindow = nullptr;
  SDL_Renderer *rawRenderer = nullptr;

  if (!SDL_CreateWindowAndRenderer(title.data(), width, height,
                                   SDL_WINDOW_RESIZABLE, &rawWindow,
                                   &rawRenderer)) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return std::unexpected<std::string>(
        std::string("Failed to create window and renderer: ") + SDL_GetError());
  }

  // Turns on VSYNC 1: Enabled (Synchronizes to your monitor's refresh rate)
  SDL_SetRenderVSync(rawRenderer, 1);

  return GraphicsModule(rawWindow, rawRenderer);
};

// This is the private constructor. In the initializer above you see that the
// window and renderer are being assigned to the vaults of the unique pointers.
// This is called the Member Initializer List
GraphicsModule::GraphicsModule(SDL_Window *window, SDL_Renderer *renderer)
    : m_window(window), m_renderer(renderer) //<-- This is the Initializer List
{
  // This is the constructor body
  // Why do this? because if you were to assign within the constructors body,
  // the compiler must create space for empty m_window and m_renderer
  // With the " : m_window(window), and m_renderer(renderer)"
  // The compiler knows to build with m_window, m_renderer already pointing to
  // the pointers passed to it.
}

// [2026-04-09] Bug Fix & Modern C++ Learning Note:
// The original destructor simply called `SDL_QuitSubSystem(SDL_INIT_VIDEO);`
// unconditionally. Because GraphicsModule is returned by value from
// `GraphicsModule::create` (inside a `std::expected`), a temporary instance of
// GraphicsModule is created and then *moved* into the return value. When that
// temporary instance goes out of scope and is destroyed, its destructor is
// called.
//
// In C++, when an object is moved (using the default move constructor
// `GraphicsModule(GraphicsModule &&) = default;`), its smart pointers (like
// `std::unique_ptr` for m_window and m_renderer) are transferred to the new
// object, and the temporary object's pointers are set to `nullptr`.
//
// If we unconditionally call `SDL_QuitSubSystem` in the destructor, the
// destruction of the temporary object will prematurely shut down the entire SDL
// Video Subsystem, destroying our window instantly! The new object receives the
// valid `m_window` pointer, but the underlying SDL window it points to has
// already been destroyed by the subsystem shutdown. The program then hangs in
// the event loop with no window.
//
// To fix this, we check if `m_window` is not null. Because a moved-from
// GraphicsModule will have a null `m_window`, this check ensures that we only
// quit the SDL subsystem when the "true" owner (the instance that still
// actually holds the window) is finally destroyed.
GraphicsModule::~GraphicsModule() {
  if (m_window) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
}
