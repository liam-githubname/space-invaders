# Source directory CMakeLists.txt - Defines the main executable target
# This follows modern CMake conventions where each directory manages its own targets - 2026-04-09

# Create the main executable - modern practice uses target-based approach
add_executable(space-invaders main.cpp)

# Modern CMake: Use target_compile_features instead of variables for C++ standard
# This sets compile features per-target rather than globally - 2026-04-08
target_compile_features(space-invaders PRIVATE cxx_std_23)

# Link against GraphicsModule library - this provides window/renderer management - 2026-04-09
# Using PRIVATE because main.cpp doesn't expose GraphicsModule in its public API (executable, not library)
# IMPORTANT: SDL3 dependencies are automatically inherited from graphics-module via PUBLIC linkage
# This is the power of modern CMake - we don't need to explicitly link SDL3 here
# Transitive dependency chain: space-invaders -> graphics-module -> SDL3::SDL3
# Added game-mechanics to link chain - the linker needs both libraries to resolve all symbols - 2026-04-15
# Dependency chain: space-invaders -> graphics-module -> SDL3::SDL3
#                   space-invaders -> game-mechanics
target_link_libraries(space-invaders PRIVATE graphics-module game-mechanics)

# Copy assets directory to build directory during build
# Assets path remains in project root following game development conventions - 2026-04-08
add_custom_command(TARGET space-invaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)
