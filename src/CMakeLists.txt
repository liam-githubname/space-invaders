# Source directory CMakeLists.txt - Defines the main executable target
# This follows modern CMake conventions where each directory manages its own targets - 2026-04-09

# Create the main executable - modern practice uses target-based approach
add_executable(space-invaders main.cpp)

# Modern CMake: Use target_compile_features instead of variables for C++ standard
# This sets compile features per-target rather than globally - 2026-04-08
target_compile_features(space-invaders PRIVATE cxx_std_23)

# Link against libraries - 2026-04-15
# graphics-module: window/renderer management (transitively provides SDL3::SDL3 and SDL3_image)
# game-engine: consolidated game logic, input, and movement systems (see lib/CMakeLists.txt for rationale)
target_link_libraries(space-invaders PRIVATE graphics-module game-engine)

# Copy assets directory to build directory during build
# Assets path remains in project root following game development conventions - 2026-04-08
add_custom_command(TARGET space-invaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/src/assets
)
