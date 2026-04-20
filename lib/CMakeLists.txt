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

# Consolidated game-engine library - merges former game-mechanics, input-system, and movement-system targets - 2026-04-15
#
# WHY: Previously these were 3 separate CMake targets (game-mechanics, input-system, movement-system), each with
# identical include_directories, compile_features, and near-identical link_libraries declarations. This produced
# significant boilerplate for targets that are tightly coupled by design - InputSystem and MovementSystem both
# #include GameMechanics.hpp and operate on GameState. At this project's scale, separate targets offered no
# real build isolation benefit (any change to GameMechanics.hpp forces recompilation of all 3 anyway), while
# making the link line in src/CMakeLists.txt grow with every new system added.
#
# By consolidating into a single target, we:
#   1. Eliminate repetitive boilerplate for include dirs, C++ standard, and transitive dependencies
#   2. Simplify the consumer link line to just 'game-engine' instead of listing every subsystem
#   3. Make adding new systems a one-line change (add AnotherSystem.cpp to the SOURCES list)
#
# If independent build isolation becomes necessary in the future (e.g. subsystems with conflicting
# dependencies), targets can be split back out. Until then, a single target is the pragmatic choice.
add_library(game-engine
    GameMechanics.cpp
    InputSystem.cpp
    MovementSystem.cpp
    RenderModule.cpp
)
target_include_directories(game-engine PUBLIC ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(game-engine PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)
target_compile_features(game-engine PUBLIC cxx_std_23)
