# Game-specific library for Space Invaders
# This target holds gameplay logic that is NOT reusable across projects - 2026-05-17
add_library(space-invaders-game
  Game.cpp
  EntityFactory.cpp
  RoundSystem.cpp
)

# Expose both lib/ (engine headers) and game/ (our own headers) to consumers - 2026-05-17
# game/ is included so src/main.cpp can write #include "Game.hpp" without relative paths.
target_include_directories(space-invaders-game PUBLIC
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/game
)

# Link engine library and graphics module - 2026-05-17
# graphics-module is linked PUBLIC because Game.cpp owns a GraphicsModule member;
# consumers of space-invaders-game (src/main.cpp) do not need to know about this
target_link_libraries(space-invaders-game PUBLIC game-engine graphics-module)

# Propagate C++23 requirement to consumers
target_compile_features(space-invaders-game PUBLIC cxx_std_23)
