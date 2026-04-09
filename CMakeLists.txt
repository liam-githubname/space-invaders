cmake_minimum_required(VERSION 3.20)
project(space-invaders LANGUAGES CXX VERSION 0.1.0)

# Modern CMake project structure: source files in src/, libraries in lib/, tests in tests/
# This follows professional C++ conventions and improves code organization - 2026-04-09
add_executable(space-invaders src/main.cpp)

# Modern CMake: Use target_compile_features instead of variables for C++ standard
# This sets compile features per-target rather than globally - 2026-04-08
target_compile_features(space-invaders PRIVATE cxx_std_23)

# Modern CMake: Use find_package with imported targets instead of pkg-config variables
# Imported targets automatically handle include directories, compile definitions, and link dependencies
# This eliminates manual management of SDL3_INCLUDE_DIRS, SDL3_LIBRARIES, etc. - 2026-04-08
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

# Modern CMake: Link against imported targets (SDL3::SDL3, SDL3_image::SDL3_image)
# Not variables. This ensures proper transitive dependencies and avoids duplicates
target_link_libraries(space-invaders PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# Copy assets directory to build directory during build
# Assets path remains in project root following game development conventions - 2026-04-08
add_custom_command(TARGET space-invaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)