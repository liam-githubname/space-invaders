# Root CMakeLists.txt - Top-level build configuration
# This follows modern CMake conventions using subdirectories for better modularity - 2026-04-09

cmake_minimum_required(VERSION 3.20)
project(space-invaders LANGUAGES CXX VERSION 0.1.0)

# Find SDL3 packages at root level - makes them available to all subdirectories
# This follows modern CMake practice where dependencies are found once at root - 2026-04-09
# IMPORTANT: Both lib/ and src/ need SDL3, so we find it once here rather than duplicating find_package calls
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

# Modern CMake project structure: libraries first, then executables, then tests
# This ensures proper dependency ordering: lib -> game -> src -> tests - 2026-05-17
# REORDERED: game/ must be processed before src/ because src/main.cpp will link
#            against the space-invaders-game target. CMake requires targets to be
#            defined before they are consumed in target_link_libraries.
add_subdirectory(lib)    # Build libraries first (other targets depend on these)
add_subdirectory(game)   # Build game-specific logic (depends on lib targets)
add_subdirectory(src)    # Build main executable (depends on game lib)
add_subdirectory(tests)  # Build tests last (depend on both libraries and executables)
