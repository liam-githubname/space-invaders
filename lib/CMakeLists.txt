# Library directory CMakeLists.txt - Defines library targets for game components
# This follows modern CMake conventions where libraries are defined separately from executables - 2026-04-09

# Define GraphicsModule library - provides window/renderer management - 2026-04-09
# This library encapsulates SDL initialization and window creation logic
add_library(graphics-module GraphicsModule.cpp)

# Set include directories PUBLIC - consumers (like src/main.cpp) automatically get this path - 2026-04-09
# This allows including "GraphicsModule.hpp" directly without relative paths like "../lib/GraphicsModule.hpp"
# Using PUBLIC because GraphicsModule.hpp is part of the public API that consumers must include
target_include_directories(graphics-module PUBLIC ${CMAKE_SOURCE_DIR}/lib)

# Link SDL3 dependencies PUBLIC - consumers automatically inherit these dependencies - 2026-04-09
# Using PUBLIC because GraphicsModule's public API exposes SDL types (SDL_Window*, SDL_Renderer*)
# This means consumers also need SDL3 linked when they call getWindow()/getRenderer()
# Without PUBLIC, consumers would get linker errors when using the public API
target_link_libraries(graphics-module PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)

# Set C++23 standard for this library - required for modern features like std::expected - 2026-04-09
# Using PUBLIC because the public API uses std::expected<GraphicsModule, std::string>
target_compile_features(graphics-module PUBLIC cxx_std_23)

# Define GameMechanics library - provides entity/component system for game state - 2026-04-15
# This library must be a separate target so the linker can find GameState symbols
# Without this, main.cpp would get "Undefined symbols" because the .cpp was never compiled
add_library(game-mechanics GameMechanics.cpp)
# Using PUBLIC so consumers (like src/main.cpp) can include "GameMechanics.hpp" without path prefixes - 2026-04-15
# IMPORTANT: Fixed typo CMAKE_SURCE_DIR -> CMAKE_SOURCE_DIR (was causing include path resolution failure) - 2026-04-15
target_include_directories(game-mechanics PUBLIC ${CMAKE_SOURCE_DIR}/lib)
# C++23 required for std::optional and other modern features used in GameMechanics - 2026-04-15
target_compile_features(game-mechanics PUBLIC cxx_std_23)
